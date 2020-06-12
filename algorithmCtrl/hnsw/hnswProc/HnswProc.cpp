//
// Created by Chunel on 2020/5/23.
// hnsw算法的封装层，对外暴漏的算法使用接口
// 这里理论上是不能有加锁操作的，所有的锁在manage这一层保存
//

#include <fstream>
#include <queue>
#include <iomanip>
#include "HnswProc.h"

using namespace std;

// 静态成员变量使用前，先初始化
HierarchicalNSW<CAISS_FLOAT>*  HnswProc::hnsw_alg_ptr_ = nullptr;
RWLock HnswProc::lock_;

inline static bool isAnnSuffix(const char *modelPath) {
    string path = string(modelPath);
    bool ret = (path.find(MODEL_SUFFIX) == path.length() - string(MODEL_SUFFIX).length());
    return ret;
}


HnswProc::HnswProc() {
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
    if (distanceType == ANN_DISTANCE_EDITION) {
        ANN_ASSERT_NOT_NULL(distFunc)    // 如果是定制距离的话，必须传距离计算函数下来
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
    this->result_.clear();

    CAISS_FUNCTION_END
}


CAISS_RET_TYPE HnswProc::train(const char* dataPath, const unsigned int maxDataSize, const CAISS_BOOL normalize, const float precision,
                             const unsigned int fastRank, const unsigned int realRank,
                             const unsigned int step, const unsigned int maxEpoch, const unsigned int showSpan) {
    CAISS_FUNCTION_BEGIN
    CAISS_ASSERT_NOT_NULL(dataPath)
    CAISS_ASSERT_NOT_NULL(this->distance_ptr_)
    CAISS_CHECK_MODE_ENABLE(CAISS_MODE_TRAIN)

    this->normalize_ = normalize;

    HnswProc::createHnswSingleton(this->distance_ptr_, maxDataSize, normalize);

    std::vector<AnnDataNode> datas;
    datas.reserve(maxDataSize);    // 提前分配好内存信息
    ret = loadDatas(dataPath, datas);
    CAISS_FUNCTION_CHECK_STATUS

    ret = trainModel(datas);
    CAISS_FUNCTION_CHECK_STATUS

    CAISS_FUNCTION_END
}


CAISS_RET_TYPE HnswProc::search(void *info, CAISS_SEARCH_TYPE searchType, const unsigned int topK) {
    CAISS_FUNCTION_BEGIN

    CAISS_ASSERT_NOT_NULL(info)
    auto ptr = HnswProc::getHnswSingleton();
    CAISS_ASSERT_NOT_NULL(ptr)
    CAISS_CHECK_MODE_ENABLE(CAISS_MODE_PROCESS)

    this->result_.clear();
    this->result_words_.clear();

    std::vector<CAISS_FLOAT> vec;
    vec.reserve(this->dim_);

    switch (searchType) {
        case CAISS_SEARCH_QUERY: {    // 如果传入的是query信息的话
            for (int i = 0; i < this->dim_; i++) {
                vec.push_back(*((CAISS_FLOAT *)info + i));
            }
            ret = normalizeNode(vec, this->dim_);    // 前面将信息转成query的形式
            break;
        }
        case CAISS_SEARCH_WORD: {    // 过传入的是word信息的话
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

    auto *query = (CAISS_FLOAT *)vec.data();
    std::priority_queue<std::pair<CAISS_FLOAT, labeltype>> predResult = ptr->searchKnn((void *)query, topK);

    ret = buildResult(query, predResult);
    CAISS_FUNCTION_CHECK_STATUS

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
    ptr->saveIndex(path);

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


CAISS_RET_TYPE HnswProc::ignore(const char *label) {
    CAISS_FUNCTION_BEGIN

    CAISS_ASSERT_NOT_NULL(label)
    // todo 逻辑待实现

    CAISS_FUNCTION_END
}


/************************ 以下是本Proc类内部函数 ************************/
/**
 * 读取文件中信息，并存至datas中
 * @param datas
 * @return
 */
CAISS_RET_TYPE HnswProc::loadDatas(const char *dataPath, vector<AnnDataNode> &datas) {
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

        AnnDataNode dataNode;
        ret = RapidJsonProc::parseInputData(line.data(), dataNode);
        CAISS_FUNCTION_CHECK_STATUS

        datas.push_back(dataNode);
    }

    CAISS_FUNCTION_END
}


CAISS_RET_TYPE HnswProc::trainModel(vector<AnnDataNode> &datas) {
    CAISS_FUNCTION_BEGIN
    auto ptr = HnswProc::getHnswSingleton();
    CAISS_ASSERT_NOT_NULL(ptr)

    unsigned int size = datas.size();
    for (unsigned int i = 0; i < size; i++) {
        ret = normalizeNode(datas[i].node, this->dim_);    // 在normalizeNode函数内部，判断是否需要归一化
        CAISS_FUNCTION_CHECK_STATUS
        ret = insertByOverwrite(datas[i].node.data(), i, (char *)datas[i].index.c_str());
        CAISS_FUNCTION_CHECK_STATUS

        if (i % 1000 == 0) {
            std::cout << "====" << i << "====" << std::endl;
        }
    }

    ptr->saveIndex(std::string(this->model_path_));
    CAISS_FUNCTION_END
}


CAISS_RET_TYPE HnswProc::buildResult(const CAISS_FLOAT *query, std::priority_queue<std::pair<CAISS_FLOAT, labeltype>>  &predResult) {
    CAISS_FUNCTION_BEGIN
    CAISS_ASSERT_NOT_NULL(query)
    auto ptr = HnswProc::getHnswSingleton();
    CAISS_ASSERT_NOT_NULL(ptr);

    std::list<AnnResultDetail> detailsList;
    while (!predResult.empty()) {
        AnnResultDetail detail;
        auto cur = predResult.top();
        detail.node = ptr->getDataByLabel<CAISS_FLOAT>(cur.second);
        detail.distance = cur.first;
        detail.index = cur.second;
        detail.label = ptr->index_lookup_.left.find(cur.second)->second;
        detailsList.push_front(detail);
        this->result_words_.push_front(detail.label);    // 保存label（词语）信息
        predResult.pop();
    }

    ret = RapidJsonProc::buildSearchResult(detailsList, this->distance_type_, this->result_);
    CAISS_FUNCTION_CHECK_STATUS

    CAISS_FUNCTION_END
}


CAISS_RET_TYPE HnswProc::loadModel(const char *modelPath) {
    CAISS_FUNCTION_BEGIN

    CAISS_ASSERT_NOT_NULL(modelPath)
    CAISS_ASSERT_NOT_NULL(this->distance_ptr_)

    HnswProc::createHnswSingleton(this->distance_ptr_, this->model_path_);    // 读取模型的时候，使用的获取方式
    this->normalize_ = HnswProc::getHnswSingleton()->normalize_;    // 保存模型的时候，会写入是否被标准化的信息

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
        case CAISS_DISTANCE_EDITION:    // todo 今后需要外部传入距离计算的函数
            this->distance_ptr_ = new EditionProductSpace(this->dim_);
            if (nullptr != distFunc) {
                this->distance_ptr_->set_dist_func((DISTFUNC<float>)distFunc);
            }
            break;
        default:
            break;
    }

    CAISS_FUNCTION_END
}


/**
 * 训练模型的时候，使用的构建方式（static成员函数）
 * @param distance_ptr
 * @param maxDataSize
 * @param normalize
 * @return
 */
CAISS_RET_TYPE HnswProc::createHnswSingleton(SpaceInterface<CAISS_FLOAT> *distance_ptr, unsigned int maxDataSize, CAISS_BOOL normalize) {
    CAISS_FUNCTION_BEGIN

    if (nullptr == HnswProc::hnsw_alg_ptr_) {
        HnswProc::lock_.writeLock();
        if (nullptr == HnswProc::hnsw_alg_ptr_) {
            HnswProc::hnsw_alg_ptr_ = new HierarchicalNSW<CAISS_FLOAT>(distance_ptr, maxDataSize, normalize);
        }
        HnswProc::lock_.writeUnlock();
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

    if (nullptr == HnswProc::hnsw_alg_ptr_) {
        HnswProc::lock_.writeLock();
        if (nullptr == HnswProc::hnsw_alg_ptr_) {
            // 这里是static函数信息，只能通过传递值下来的方式实现
            HnswProc::hnsw_alg_ptr_ = new HierarchicalNSW<CAISS_FLOAT>(distance_ptr, modelPath);
        }
        HnswProc::lock_.writeUnlock();
    }

    CAISS_FUNCTION_END
}

HierarchicalNSW<CAISS_FLOAT> *HnswProc::getHnswSingleton() {
    return HnswProc::hnsw_alg_ptr_;
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


