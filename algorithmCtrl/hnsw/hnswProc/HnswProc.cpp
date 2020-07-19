//
// Created by Chunel on 2020/5/23.
// hnsw算法的封装层，对外暴漏的算法使用接口
// 这里理论上是不能有加锁操作的，所有的锁在manage这一层保存
//

#include <algorithm>
#include <queue>
#include <iomanip>
#include "HnswProc.h"

#ifdef _USE_OPENMP_
    #include <omp.h>    // 如果有openmp加速
#endif

using namespace std;

// 静态成员变量使用前，先初始化
HierarchicalNSW<CAISS_FLOAT>* HnswProc::hnsw_algo_ptr_ = nullptr;
RWLock HnswProc::hnsw_algo_lock_;

RWLock AlgorithmProc::trie_lock_;
TrieProc* AlgorithmProc::ignore_trie_ptr_;

inline static bool isAnnSuffix(const char *modelPath) {
    string path = string(modelPath);
    bool ret = (path.find(MODEL_SUFFIX) == path.length() - string(MODEL_SUFFIX).length());
    return ret;
}


inline static bool isWordSearchType(CAISS_SEARCH_TYPE searchType) {
    bool ret = false;
    if (CAISS_SEARCH_WORD == searchType
        || CAISS_LOOP_WORD == searchType) {
        ret = true;
    }
    return ret;
}

inline static bool isAnnSearchType(CAISS_SEARCH_TYPE searchType) {
    // 判定是否是快速查询类型
    bool ret = false;
    if (CAISS_SEARCH_WORD == searchType
        || CAISS_SEARCH_QUERY == searchType) {
        ret = true;
    }
    return ret;
}


HnswProc::HnswProc() {
    this->neighbors_ = 0;
    this->distance_ptr_ = nullptr;
}


HnswProc::~HnswProc() {
    this->reset();
}


/************************ 以下是重写的算法基类接口内容 ************************/
CAISS_RET_TYPE
HnswProc::init(const CAISS_MODE mode, const CAISS_DISTANCE_TYPE distanceType, const unsigned int dim, const char *modelPath,
               const CAISS_DIST_FUNC distFunc = nullptr) {
    CAISS_FUNCTION_BEGIN
    CAISS_ASSERT_NOT_NULL(modelPath);
    if (distanceType == CAISS_DISTANCE_EDITION) {
        CAISS_ASSERT_NOT_NULL(distFunc)    // 如果是定制距离的话，必须传距离计算函数下来
    }

    reset();    // 清空所有数据信息

    this->dim_ = dim;
    this->cur_mode_ = mode;
    // 如果是train模式，则是需要保存到这里；如果process模式，则是读取模型
    this->model_path_ = isAnnSuffix(modelPath) ? (string(modelPath)) : (string(modelPath) + MODEL_SUFFIX);
    this->distance_type_ = distanceType;
    createDistancePtr(distFunc);

    if (this->cur_mode_ == CAISS_MODE_PROCESS) {
        ret = loadModel(modelPath);    // 如果是处理模式的话，则读取模型内容信息
        CAISS_FUNCTION_CHECK_STATUS
    }

    CAISS_FUNCTION_END
}


CAISS_RET_TYPE HnswProc::reset() {
    CAISS_FUNCTION_BEGIN

    CAISS_DELETE_PTR(distance_ptr_)
    this->dim_ = 0;
    this->cur_mode_ = CAISS_MODE_DEFAULT;
    this->normalize_ = 0;
    this->neighbors_ = 0;
    this->result_.clear();

    CAISS_FUNCTION_END
}


CAISS_RET_TYPE HnswProc::train(const char *dataPath, const unsigned int maxDataSize, const CAISS_BOOL normalize,
                               const unsigned int maxIndexSize, const float precision, const unsigned int fastRank,
                               const unsigned int realRank, const unsigned int step, const unsigned int maxEpoch,
                               const unsigned int showSpan) {
    CAISS_FUNCTION_BEGIN
    CAISS_ASSERT_NOT_NULL(dataPath)
    CAISS_ASSERT_NOT_NULL(this->distance_ptr_)
    CAISS_CHECK_MODE_ENABLE(CAISS_MODE_TRAIN)

    // 设定训练参数
    this->normalize_ = normalize;
    std::vector<CaissDataNode> datas;
    datas.reserve(maxDataSize);    // 提前分配好内存信息

    CAISS_ECHO("start load datas from [%s].", dataPath);
    ret = loadDatas(dataPath, datas);
    CAISS_FUNCTION_CHECK_STATUS

    HnswProc::createHnswSingleton(this->distance_ptr_, maxDataSize, normalize, maxIndexSize);
    HnswTrainParams params(step);

    unsigned int epoch = 0;
    while (epoch < maxEpoch) {    // 如果批量走完了，则默认返回
        CAISS_ECHO("start to train caiss model for [%d] in [%d] epochs.", ++epoch, maxEpoch);
        ret = trainModel(datas, showSpan);
        CAISS_FUNCTION_CHECK_STATUS
        CAISS_ECHO("model build finished, check model precision automatic, please wait for a moment...");

        float calcPrecision = 0.0f;
        ret = checkModelPrecisionEnable(precision, fastRank, realRank, datas, calcPrecision);
        if (CAISS_RET_OK == ret) {    // 如果训练的准确度符合要求，则直接退出
            CAISS_ECHO("train success, precision is [%0.4f] , model is saved to path [%s].", calcPrecision,
                       this->model_path_.c_str());
            break;
        } else if (CAISS_RET_WARNING == ret) {
            float span = precision - calcPrecision;
            CAISS_ECHO("warning, the model's precision is not suitable, span = [%f], train again automatic.", span);
            params.update(span);
            destroyHnswSingleton();    // 销毁句柄信息，重新训练
            createHnswSingleton(this->distance_ptr_, maxDataSize, normalize, maxIndexSize, params.neighborNums, params.efSearch, params.efConstructor);
        }
    }

    CAISS_FUNCTION_CHECK_STATUS    // 如果是precision达不到要求，则返回警告信息
    CAISS_FUNCTION_END
}


CAISS_RET_TYPE HnswProc::search(void *info,
                                const CAISS_SEARCH_TYPE searchType,
                                const unsigned int topK,
                                const unsigned int filterEditDistance,
                                const CAISS_SEARCH_CALLBACK searchCBFunc,
                                const void *cbParams) {
    CAISS_FUNCTION_BEGIN

    CAISS_ASSERT_NOT_NULL(info)
    CAISS_CHECK_MODE_ENABLE(CAISS_MODE_PROCESS)

    /* 将信息清空 */
    this->result_.clear();
    this->result_words_.clear();
    this->result_distance_.clear();
    CAISS_BOOL isGet = CAISS_FALSE;

    if (isWordSearchType(searchType)) {
        ret = searchInLruCache((const char *)info, searchType, topK, isGet);    // 如果查询的是单词，则先进入cache中获取
        CAISS_FUNCTION_CHECK_STATUS
    }

    if (!isGet) {    // 如果没有在cache中获取到信息
        ret = innerSearchResult(info, searchType, topK, filterEditDistance);
        CAISS_FUNCTION_CHECK_STATUS
    }

    if (nullptr != searchCBFunc) {
        searchCBFunc(this->result_words_, this->result_distance_, cbParams);    // 可以看看params如何传递下来比较好一点
    }

    this->last_topK_ = topK;    // 查询完毕之后，记录当前的topK信息
    this->last_search_type_ = searchType;
    if (isWordSearchType(searchType)) {
        this->lru_cache_.put(std::string((char *)info), this->result_);
    }

    CAISS_FUNCTION_END
}


CAISS_RET_TYPE HnswProc::insert(CAISS_FLOAT *node, const char *index, CAISS_INSERT_TYPE insertType) {
    CAISS_FUNCTION_BEGIN
    CAISS_ASSERT_NOT_NULL(node)
    CAISS_ASSERT_NOT_NULL(index)
    auto ptr = HnswProc::getHnswSingleton();
    CAISS_ASSERT_NOT_NULL(ptr)

    CAISS_CHECK_MODE_ENABLE(CAISS_MODE_PROCESS)

    unsigned int curCount = ptr->cur_element_count_;
    if (curCount >= ptr->max_elements_) {
        return CAISS_RET_MODEL_SIZE;    // 超过模型的最大尺寸了
    }

    std::vector<CAISS_FLOAT> vec;
    vec.reserve(this->dim_);
    for (int i = 0; i < this->dim_; i++) {
        vec.push_back(node[i]);
    }

    ret = normalizeNode(vec, this->dim_);
    CAISS_FUNCTION_CHECK_STATUS

    switch (insertType) {
        case CAISS_INSERT_OVERWRITE:
            ret = insertByOverwrite(vec.data(), curCount, index);
            break;
        case CAISS_INSERT_DISCARD:
            ret = insertByDiscard(vec.data(), curCount, index);
            break;
        default:
            ret = CAISS_RET_PARAM;
            break;
    }

    CAISS_FUNCTION_CHECK_STATUS

    this->last_topK_ = 0;    // 如果插入成功，则重新记录topK信息
    this->last_search_type_ = CAISS_SEARCH_DEFAULT;
    CAISS_FUNCTION_END
}


CAISS_RET_TYPE HnswProc::save(const char *modelPath) {
    CAISS_FUNCTION_BEGIN
    auto ptr = HnswProc::getHnswSingleton();
    CAISS_ASSERT_NOT_NULL(ptr)

    std::string path;
    if (nullptr == modelPath) {
        path = this->model_path_;    // 如果传入的值为空，则保存当前的模型
    } else {
        path = isAnnSuffix(modelPath) ? string(modelPath) : (string(modelPath) + MODEL_SUFFIX);
    }

    remove(path.c_str());    // 如果有的话，就删除
    list<string> ignoreList = AlgorithmProc::getIgnoreTrie()->getAllWords();
    ptr->saveIndex(path, ignoreList);

    CAISS_FUNCTION_END
}


CAISS_RET_TYPE HnswProc::getResultSize(unsigned int &size) {
    CAISS_FUNCTION_BEGIN
    CAISS_CHECK_MODE_ENABLE(CAISS_MODE_PROCESS)

    size = this->result_.size();

    CAISS_FUNCTION_END
}


CAISS_RET_TYPE HnswProc::getResult(char *result, unsigned int size) {
    CAISS_FUNCTION_BEGIN
    CAISS_ASSERT_NOT_NULL(result)
    CAISS_CHECK_MODE_ENABLE(CAISS_MODE_PROCESS)

    memset(result, 0, size);
    memcpy(result, this->result_.data(), this->result_.size());

    CAISS_FUNCTION_END
}


CAISS_RET_TYPE HnswProc::ignore(const char *label, const bool isIgnore) {
    CAISS_FUNCTION_BEGIN
    CAISS_ASSERT_NOT_NULL(label)
    CAISS_CHECK_MODE_ENABLE(CAISS_MODE_PROCESS)    // process 模式下，才能进行

    string info = label;
    if (isIgnore) {
        AlgorithmProc::AlgorithmProc::getIgnoreTrie()->insert(info);    // 对于外部的 ignore 当前单词，相当于是在字典树中，加入这个词语
    } else {
        AlgorithmProc::AlgorithmProc::getIgnoreTrie()->eraser(info);    // 对于外部的 not-ignore，相当于是在字典树中，
    }

    this->last_topK_ = 0;    // 如果插入成功，则重新记录topK信息
    this->last_search_type_ = CAISS_SEARCH_DEFAULT;

    CAISS_FUNCTION_END
}


/************************ 以下是本Proc类内部函数 ************************/
/**
 * 读取文件中信息，并存至datas中
 * @param datas
 * @return
 */
CAISS_RET_TYPE HnswProc::loadDatas(const char *dataPath, vector<CaissDataNode> &datas) {
    CAISS_FUNCTION_BEGIN
    CAISS_ASSERT_NOT_NULL(dataPath);

    std::ifstream in(dataPath);
    if (!in) {
        return CAISS_RET_PATH;
    }

    std::string line;
    while (getline(in, line)) {
        if (0 == line.length()) {
            continue;    // 排除空格的情况
        }

        CaissDataNode dataNode;
        ret = RapidJsonProc::parseInputData(line.data(), dataNode);
        CAISS_FUNCTION_CHECK_STATUS

        ret = normalizeNode(dataNode.node, this->dim_);    // 在normalizeNode函数内部，判断是否需要归一化
        CAISS_FUNCTION_CHECK_STATUS

        datas.push_back(dataNode);
    }

    in.close();
    CAISS_FUNCTION_END
}


CAISS_RET_TYPE HnswProc::trainModel(std::vector<CaissDataNode> &datas, const unsigned int showSpan) {
    CAISS_FUNCTION_BEGIN
    auto ptr = HnswProc::getHnswSingleton();
    CAISS_ASSERT_NOT_NULL(ptr)

    unsigned int size = datas.size();
    for (unsigned int i = 0; i < size; i++) {
        ret = insertByOverwrite(datas[i].node.data(), i, (char *)datas[i].index.c_str());
        CAISS_FUNCTION_CHECK_STATUS

        if (showSpan != 0 && i % showSpan == 0) {
            CAISS_ECHO("train [%d] node, total size is [%d].", i, (int)datas.size());
        }
    }

    remove(this->model_path_.c_str());
    ptr->saveIndex(std::string(this->model_path_), std::list<string>());    // 训练的时候，传入的是空的ignore链表
    CAISS_FUNCTION_END
}


CAISS_RET_TYPE HnswProc::buildResult(const CAISS_FLOAT *query, const CAISS_SEARCH_TYPE searchType,
                                     HNSW_RET_TYPE &predResult) {
    CAISS_FUNCTION_BEGIN
    CAISS_ASSERT_NOT_NULL(query)
    auto ptr = HnswProc::getHnswSingleton();
    CAISS_ASSERT_NOT_NULL(ptr);

    std::list<CaissResultDetail> detailsList;
    while (!predResult.empty()) {
        CaissResultDetail detail;
        auto cur = predResult.top();
        predResult.pop();
        detail.node = ptr->getDataByLabel<CAISS_FLOAT>(cur.second);
        detail.distance = cur.first;
        detail.index = cur.second;
        detail.label = ptr->index_lookup_.left.find(cur.second)->second;    // 这里的label，是单词信息

        detailsList.push_front(detail);
        this->result_words_.push_front(detail.label);    // 保存label（词语）信息
        this->result_distance_.push_front(detail.distance);    // 保存对应的距离信息
    }

    std::string type;
    if (isAnnSearchType(searchType)) {
        type = "ann_search";
    } else {
        type = "force_loop";
    }


    ret = RapidJsonProc::buildSearchResult(detailsList, this->distance_type_, this->result_, type);
    CAISS_FUNCTION_CHECK_STATUS

    CAISS_FUNCTION_END
}


CAISS_RET_TYPE HnswProc::loadModel(const char *modelPath) {
    CAISS_FUNCTION_BEGIN

    CAISS_ASSERT_NOT_NULL(modelPath)
    CAISS_ASSERT_NOT_NULL(this->distance_ptr_)

    HnswProc::createHnswSingleton(this->distance_ptr_, this->model_path_);    // 读取模型的时候，使用的获取方式
    this->normalize_ = HnswProc::getHnswSingleton()->normalize_;    // 保存模型的时候，会写入是否被标准化的信息
    this->neighbors_ = HnswProc::getHnswSingleton()->ef_construction_;

    CAISS_FUNCTION_END
}


CAISS_RET_TYPE HnswProc::createDistancePtr(CAISS_DIST_FUNC distFunc) {
    CAISS_FUNCTION_BEGIN

    CAISS_DELETE_PTR(this->distance_ptr_)    // 先删除，确保不会出现重复new的情况
    switch (this->distance_type_) {
        case CAISS_DISTANCE_EUC :
            this->distance_ptr_ = new L2Space(this->dim_);
            break;
        case CAISS_DISTANCE_INNER:
            this->distance_ptr_ = new InnerProductSpace(this->dim_);
            break;
        case CAISS_DISTANCE_EDITION:
            this->distance_ptr_ = new EditionProductSpace(this->dim_);
            this->distance_ptr_->set_dist_func((DISTFUNC<float>)distFunc);
            break;
        default:
            break;
    }

    CAISS_FUNCTION_END
}


/**
 * 现在每个lru，都是针对句柄独立的
 * @param word
 * @param searchType
 * @param topK
 * @param isGet
 * @return
 */
CAISS_RET_TYPE HnswProc::searchInLruCache(const char *word, const CAISS_SEARCH_TYPE searchType, const unsigned int topK,
                                          CAISS_BOOL &isGet) {
    CAISS_FUNCTION_BEGIN
    CAISS_ASSERT_NOT_NULL(word)

    isGet = CAISS_FALSE;
    if (topK == last_topK_ && searchType == last_search_type_) {    // 查询的还是上次的topK，并且查詢的方式还是一致的话
        std::string&& result = lru_cache_.get(std::string(word));
        if (!result.empty()) {
            this->result_ = (result);
            ret = RapidJsonProc::parseResult(this->result_, this->result_words_, this->result_distance_);
            CAISS_FUNCTION_CHECK_STATUS
            isGet = CAISS_TRUE;    // 如果有值，直接给result赋值
        }
    } else {
        lru_cache_.clear();    // 如果topK有变动，或者有信息插入的话，清空缓存信息
    }

    CAISS_FUNCTION_END
}


CAISS_RET_TYPE
HnswProc::filterByRules(void *info, const CAISS_SEARCH_TYPE searchType, HNSW_RET_TYPE &result, unsigned int topK,
                        const unsigned int filterEditDistance) {
    CAISS_FUNCTION_BEGIN
    if (result.size() <= topK) {
        return CAISS_RET_OK;    // 召回的少了，不需要做过滤信息了
    }

    // 今后可能有多种规则
    ret = filterByEditDistance(info, searchType, result, filterEditDistance);
    CAISS_FUNCTION_CHECK_STATUS

    ret = filterByIgnoreTrie(result);
    CAISS_FUNCTION_CHECK_STATUS

    // 所有的情况都过滤完了之后，保证不会超过topK个
    while (result.size() > topK) {
        result.pop();
    }

    CAISS_FUNCTION_END
}


CAISS_RET_TYPE
HnswProc::filterByEditDistance(void *info, CAISS_SEARCH_TYPE searchType, HNSW_RET_TYPE &result,
                               unsigned int filterEditDistance) {
    CAISS_FUNCTION_BEGIN
    CAISS_ASSERT_NOT_NULL(info)

    if (!isWordSearchType(searchType)    // 如果不是根据word查询，则不需要走这一步
        || (CAISS_MIN_EDIT_DISTANCE == filterEditDistance)) {   // 值=-1，不需要根据编辑距离来过滤
        return CAISS_RET_OK;
    }

    if (CAISS_MAX_EDIT_DISTANCE < filterEditDistance) {
        return CAISS_RET_PARAM;    // 如果值设置的太大了，则返回参数校验错误
    }

    auto ptr = HnswProc::getHnswSingleton();
    CAISS_ASSERT_NOT_NULL(ptr)

    string word = std::string((char *)info);    // 已经确定是查词语类型的了
    HNSW_RET_TYPE resultBackUp;

    while (!result.empty()) {
        auto cur = result.top();
        result.pop();
        string candWord = ptr->index_lookup_.left.find(cur.second)->second;    // 这里的label，是单词信息
        if (EditDistanceProc::BeyondEditDistance(candWord, word, filterEditDistance)) {
            resultBackUp.push(cur);    // 仅添加超过范围的
        }
    }

    result = resultBackUp;
    CAISS_FUNCTION_END
}


/**
 * 通过忽略trie来做过滤
 * @param info
 * @param searchType
 * @param result
 * @return
 */
CAISS_RET_TYPE HnswProc::filterByIgnoreTrie(HNSW_RET_TYPE &result) {
    CAISS_FUNCTION_BEGIN
    CAISS_ASSERT_NOT_NULL(AlgorithmProc::getIgnoreTrie())

    auto ptr = HnswProc::getHnswSingleton();
    CAISS_ASSERT_NOT_NULL(ptr)

    HNSW_RET_TYPE resultBackUp;

    while (!result.empty()) {
        auto cur = result.top();
        result.pop();
        string candWord = ptr->index_lookup_.left.find(cur.second)->second;
        if (!AlgorithmProc::getIgnoreTrie()->find(candWord)) {
            resultBackUp.push(cur);    // 如果这些词语，不在过滤trie树上，就添加进来
        }
    }

    result = resultBackUp;

    CAISS_FUNCTION_END
}



/**
 * 训练模型的时候，使用的构建方式（static成员函数）
 * @param distance_ptr
 * @param maxDataSize
 * @param normalize
 * @return
 */
CAISS_RET_TYPE HnswProc::createHnswSingleton(SpaceInterface<CAISS_FLOAT>* distance_ptr, unsigned int maxDataSize, CAISS_BOOL normalize,
                                              const unsigned int maxIndexSize, const unsigned int maxNeighbor, const unsigned int efSearch, const unsigned int efConstruction) {
    CAISS_FUNCTION_BEGIN

    if (nullptr == HnswProc::hnsw_algo_ptr_) {
        HnswProc::hnsw_algo_lock_.writeLock();
        if (nullptr == HnswProc::hnsw_algo_ptr_) {
            HnswProc::hnsw_algo_ptr_ = new HierarchicalNSW<CAISS_FLOAT>(distance_ptr, maxDataSize, normalize, maxIndexSize, maxNeighbor, efSearch, efConstruction);
        }
        HnswProc::hnsw_algo_lock_.writeUnlock();
    }

    CAISS_FUNCTION_END
}

/**
 * 加载模型的时候，使用的构建方式（static成员函数）
 * @param distance_ptr
 * @param modelPath
 * @return
 */
CAISS_RET_TYPE HnswProc::createHnswSingleton(SpaceInterface<CAISS_FLOAT> *distance_ptr, const std::string &modelPath) {
    CAISS_FUNCTION_BEGIN

    if (nullptr == HnswProc::hnsw_algo_ptr_) {
        HnswProc::hnsw_algo_lock_.writeLock();
        if (nullptr == HnswProc::hnsw_algo_ptr_) {
            // 这里是static函数信息，只能通过传递值下来的方式实现
            HnswProc::hnsw_algo_ptr_ = new HierarchicalNSW<CAISS_FLOAT>(distance_ptr, modelPath, AlgorithmProc::getIgnoreTrie());
        }
        HnswProc::hnsw_algo_lock_.writeUnlock();
    }

    CAISS_FUNCTION_END
}


CAISS_RET_TYPE HnswProc::destroyHnswSingleton() {
    CAISS_FUNCTION_BEGIN

    HnswProc::hnsw_algo_lock_.writeLock();
    CAISS_DELETE_PTR(HnswProc::hnsw_algo_ptr_);
    HnswProc::hnsw_algo_lock_.writeUnlock();

    CAISS_FUNCTION_END
}



HierarchicalNSW<CAISS_FLOAT> *HnswProc::getHnswSingleton() {
    return HnswProc::hnsw_algo_ptr_;
}


CAISS_RET_TYPE HnswProc::insertByOverwrite(CAISS_FLOAT *node, unsigned int label, const char *index) {
    CAISS_FUNCTION_BEGIN

    CAISS_ASSERT_NOT_NULL(node)    // 传入的信息，已经是normalize后的信息了
    CAISS_ASSERT_NOT_NULL(index)
    auto ptr = HnswProc::getHnswSingleton();
    CAISS_ASSERT_NOT_NULL(ptr);

    if (-1 == ptr->findWordLabel(index)) {
        // 返回-1，表示没找到对应的信息，如果不存在，则插入内容
        ret = ptr->addPoint(node, label, index);
    } else {
        // 如果被插入过了，则覆盖之前的内容，覆盖的时候，不需要考虑label的值，因为在里面，可以通过index获取
        ret = ptr->overwriteNode(node, index);
    }
    CAISS_FUNCTION_CHECK_STATUS

    CAISS_FUNCTION_END
}


CAISS_RET_TYPE HnswProc::insertByDiscard(CAISS_FLOAT *node, unsigned int label, const char *index) {
    CAISS_FUNCTION_BEGIN

    CAISS_ASSERT_NOT_NULL(node)
    CAISS_ASSERT_NOT_NULL(index)
    auto ptr = HnswProc::getHnswSingleton();
    CAISS_ASSERT_NOT_NULL(ptr)

    bool bret = ptr->findWordLabel(index);
    if (!bret) {
        // 如果不存在，则直接添加；如果存在，则不进入此逻辑，直接返回
        ret = ptr->addPoint(node, label, index);
        CAISS_FUNCTION_CHECK_STATUS
    }

    CAISS_FUNCTION_END
}


/**
 * 内部真实查询信息的时候，使用的函数。可以确保不用进入process状态，也可以查询
 * @param info
 * @param searchType
 * @param topK
 * @param filterEditDistance
 * @return
 */
CAISS_RET_TYPE HnswProc::innerSearchResult(void *info, const CAISS_SEARCH_TYPE searchType, const unsigned int topK,
                                           const unsigned int filterEditDistance) {
    CAISS_FUNCTION_BEGIN

    CAISS_ASSERT_NOT_NULL(info)
    auto ptr = HnswProc::getHnswSingleton();
    CAISS_ASSERT_NOT_NULL(ptr)

    std::vector<CAISS_FLOAT> vec;
    vec.reserve(this->dim_);

    switch (searchType) {
        case CAISS_SEARCH_QUERY:
        case CAISS_LOOP_QUERY: {    // 如果传入的是query信息的话
            for (int i = 0; i < this->dim_; i++) {
                vec.push_back(*((CAISS_FLOAT *)info + i));
            }
            ret = normalizeNode(vec, this->dim_);    // 前面将信息转成query的形式
            break;
        }
        case CAISS_SEARCH_WORD:
        case CAISS_LOOP_WORD: {    // 过传入的是word信息的话
            int label = ptr->findWordLabel((const char *)info);
            if (-1 != label) {
                vec = ptr->getDataByLabel<CAISS_FLOAT>(label);    // 找到word的情况，这种情况下，不需要做normalize。因为存入的时候，已经设定好了
            } else {
                ret = CAISS_RET_NO_WORD;    // 没有找到word的情况
            }
            break;
        }
        default:
            ret = CAISS_RET_PARAM;
            break;
    }

    CAISS_FUNCTION_CHECK_STATUS

    unsigned int queryTopK = std::max(topK*7, this->neighbors_);    // 表示7分(*^▽^*)
    auto *query = (CAISS_FLOAT *)vec.data();
    HNSW_RET_TYPE&& result = isAnnSearchType(searchType)
            ? ptr->searchKnn((void *)query, queryTopK) : ptr->forceLoop((void *)query, queryTopK);

    // 需要加入一步过滤机制
    ret = filterByRules(info, searchType, result, topK, filterEditDistance);
    CAISS_FUNCTION_CHECK_STATUS

    ret = buildResult(query, searchType, result);
    CAISS_FUNCTION_CHECK_STATUS

    CAISS_FUNCTION_END;
}


CAISS_RET_TYPE HnswProc::checkModelPrecisionEnable(const float targetPrecision, const unsigned int fastRank, const unsigned int realRank,
                                                   const vector<CaissDataNode> &datas, float &calcPrecision) {
    CAISS_FUNCTION_BEGIN
    auto ptr = HnswProc::getHnswSingleton();
    CAISS_ASSERT_NOT_NULL(ptr)

    unsigned int suitableTimes = 0;
    unsigned int calcTimes = min((int)datas.size(), 10000);    // 最多1000次比较

    {
        #ifdef _USE_OPENMP_
            #pragma omp parallel for num_threads(4) reduction(+:suitableTimes)
        #endif
        for (unsigned int i = 0; i < calcTimes; i++) {
            auto fastResult = ptr->searchKnn((void *)datas[i].node.data(), fastRank);    // 记住，fastResult是倒叙的
            auto realResult = ptr->forceLoop((void *)datas[i].node.data(), realRank);
            float fastFarDistance = fastResult.top().first;
            float realFarDistance = realResult.top().first;

            if (abs(fastFarDistance - realFarDistance) < 0.000002f) {    // 这里近似小于
                suitableTimes++;
            }
        }
    }

    calcPrecision = (float)suitableTimes / (float)calcTimes;
    ret = (calcPrecision >= targetPrecision) ? CAISS_RET_OK : CAISS_RET_WARNING;
    CAISS_FUNCTION_CHECK_STATUS

    CAISS_FUNCTION_END
}



