/***************************
@Author: Chunel
@Contact: chunel@foxmail.com
@File: MrptProc.cpp
@Time: 2020/12/4 7:42 下午
@Desc:
***************************/

#include "MrptProc.h"

RWLock MrptProc::bimap_lock_;
BOOST_BIMAP MrptProc::index_label_lookup_;    // left是unsigned int，right是string

Singleton<MrptModelHead> MrptProc::mrpt_model_head_;
Singleton<Mrpt> MrptProc::mrpt_algo_;

MrptProc::MrptProc() {

}

MrptProc::~MrptProc() {
    reset();
}

CAISS_STATUS MrptProc::init(CAISS_MODE mode, CAISS_DISTANCE_TYPE distanceType,
                            unsigned int dim, const char *modelPath, CAISS_DIST_FUNC distFunc) {
    CAISS_FUNCTION_BEGIN
    CAISS_ASSERT_NOT_NULL(modelPath);

    this->dim_ = dim;
    this->cur_mode_ = mode;
    this->model_path_ = isAnnSuffix(modelPath) ? (string(modelPath)) : (string(modelPath) + MODEL_SUFFIX);
    this->distance_type_ = distanceType;
    MrptProc::mrpt_model_head_.create();

    if (this->cur_mode_ == CAISS_MODE_PROCESS) {
        // 如果是查询模式，则直接读取模型
        ret = loadModel();
        CAISS_FUNCTION_CHECK_STATUS
    }

    CAISS_FUNCTION_END
}

// train_mode
CAISS_STATUS MrptProc::train(const char *dataPath, unsigned int maxDataSize, CAISS_BOOL normalize,
                   unsigned int maxIndexSize, float precision, unsigned int fastRank,
                   unsigned int realRank, unsigned int step, unsigned int maxEpoch,
                   unsigned int showSpan) {
    CAISS_FUNCTION_BEGIN
    CAISS_ASSERT_NOT_NULL(dataPath)
    CAISS_CHECK_MODE_ENABLE(CAISS_MODE_TRAIN)

    normalize_ = normalize;

    MrptModelHead *headPtr = MrptProc::mrpt_model_head_.get();
    CAISS_ASSERT_NOT_NULL(headPtr)

    headPtr->modelTag = MRPT_MODEL_TAG;
    headPtr->maxDataSize = (int)maxDataSize;    // 模型头需要兼容模型逻辑，故选用int版本
    headPtr->dim = (int)this->dim_;

    CAISS_ECHO("start load data from [%s].", dataPath);
    ret = loadDatas(dataPath);
    CAISS_FUNCTION_CHECK_STATUS

    CAISS_ECHO("load data finished, begin to train model.");

    // 构造mrpt模型句柄信息
    ret = MrptProc::mrpt_algo_.create(headPtr->modelData.data(),
                                      headPtr->dim,
                                      headPtr->maxDataSize);
    CAISS_FUNCTION_CHECK_STATUS

    auto ptr = MrptProc::mrpt_algo_.get();
    CAISS_ASSERT_NOT_NULL(ptr)

    CAISS_STRING_ARRAY labels;
    ret = getAllLabels(labels);
    CAISS_FUNCTION_CHECK_STATUS

    ptr->grow_autotune((double)precision, (int)fastRank);    // 自动训练模型
    remove(this->model_path_.c_str());    // 如果有的话，就删除
    if (!ptr->save(this->model_path_.c_str(), (int)maxDataSize, (int)maxIndexSize, labels)) {
        // 保存模型，如果失败，肯定是路径有问题
        return CAISS_RET_PATH;
    }

    CAISS_ECHO("model is saved to path [%s].", this->model_path_.c_str());

    CAISS_FUNCTION_END
}

// process_mode
CAISS_STATUS MrptProc::search(void *info, CAISS_SEARCH_TYPE searchType,
                              unsigned int topK, unsigned int filterEditDistance,
                              CAISS_SEARCH_CALLBACK searchCBFunc, const void *cbParams) {
    CAISS_FUNCTION_BEGIN

    CAISS_ASSERT_NOT_NULL(info)
    CAISS_CHECK_MODE_ENABLE(CAISS_MODE_PROCESS)

    /* 将信息清空 */
    this->result_.clear();
    this->word_details_map_.clear();

    if (this->last_topK_ != topK || this->last_search_type_ != searchType) {
        // 如果跟上次查询的条件不同，则lru信息直接失效
        // 每个算法线程的算法函数，有自己的句柄
        this->lru_cache_.clear();
    }

    ret = innerSearchResult(info, searchType, topK, filterEditDistance);
    CAISS_FUNCTION_CHECK_STATUS

    processCallBack(searchCBFunc, cbParams);

    this->last_topK_ = topK;    // 查询完毕之后，记录当前的topK信息
    this->last_search_type_ = searchType;

    CAISS_FUNCTION_END
}

CAISS_STATUS MrptProc::insert(CAISS_FLOAT *node,
                              const char *index,
                              CAISS_INSERT_TYPE insertType) {
    CAISS_FUNCTION_BEGIN

    CAISS_FUNCTION_NO_SUPPORT

    CAISS_FUNCTION_END
}

CAISS_STATUS MrptProc::save(const char *modelPath) {
    CAISS_FUNCTION_BEGIN

    CAISS_FUNCTION_NO_SUPPORT

    CAISS_FUNCTION_END
}

CAISS_STATUS MrptProc::ignore(const char *label,
                              CAISS_BOOL isIgnore) {
    CAISS_FUNCTION_BEGIN

    CAISS_FUNCTION_NO_SUPPORT

    CAISS_FUNCTION_END
}


/**
 * 根据传入的类型，在bimap中查询结果信息
 * @param type
 * @param index 位置信息，例：1
 * @param label 标签信息，例：the
 * @return
 */
CAISS_STATUS MrptProc::processIndexLabel(BIMAP_PROCESS_TYPE type,
                                         unsigned int &index,
                                         std::string &label) {
    CAISS_FUNCTION_BEGIN
    switch (type) {
        case BIMAP_INSERT: {
            // 插入的时候加写锁
            MrptProc::bimap_lock_.writeLock();
            MrptProc::index_label_lookup_.insert(BOOST_BIMAP::value_type(index, label));
            MrptProc::bimap_lock_.writeUnlock();
        }
            break;
        case BIMAP_FIND_INDEX: {
            MrptProc::bimap_lock_.readLock();
            auto result = MrptProc::index_label_lookup_.left.find(index);
            if (result != MrptProc::index_label_lookup_.left.end()) {
                label = result->get_right();
            } else {
                ret = CAISS_RET_WARNING;    // 如果没有找到，返回告警信息
            }
            MrptProc::bimap_lock_.readUnlock();
        }
            break;
        case BIMAP_FIND_LABEL: {
            MrptProc::bimap_lock_.readLock();
            auto result = MrptProc::index_label_lookup_.right.find(label);
            if (result != MrptProc::index_label_lookup_.right.end()){
                index = result->get_left();
            } else {
                ret = CAISS_RET_WARNING;
            }
            MrptProc::bimap_lock_.readUnlock();
        }
            break;
        case BIMAP_DELETE_INDEX: {
            MrptProc::bimap_lock_.writeLock();
            if (0 == MrptProc::index_label_lookup_.left.erase(index)) {
                // 返回值为0，表示未删除成功（如bimap中无此值信息）；为1则表示删除成功
                ret = CAISS_RET_WARNING;
            }
            MrptProc::bimap_lock_.writeUnlock();
        }
            break;
        case BIMAP_DELETE_LABEL: {
            MrptProc::bimap_lock_.writeLock();
            if (0 == MrptProc::index_label_lookup_.right.erase(label)) {
                ret = CAISS_RET_WARNING;
            }
            MrptProc::bimap_lock_.writeUnlock();
        }
            break;
        default:
            ret = CAISS_RET_PARAM;
            break;
    }

    CAISS_FUNCTION_CHECK_STATUS

    CAISS_FUNCTION_END
}


CAISS_STATUS MrptProc::getAllLabels(std::vector<std::string>& labels) {
    CAISS_FUNCTION_BEGIN

    MrptProc::bimap_lock_.readLock();
    labels.clear();
    for (const auto &x : MrptProc::index_label_lookup_) {
        labels.push_back(x.get_right());
    }
    MrptProc::bimap_lock_.readUnlock();

    CAISS_FUNCTION_END
}


/************************ 以下是重写的算法基类接口内容 ************************/
CAISS_STATUS MrptProc::reset() {
    CAISS_FUNCTION_BEGIN
    this->model_path_.clear();
    MrptProc::mrpt_algo_.destroy();
    MrptProc::mrpt_model_head_.destroy();

    CAISS_FUNCTION_END
}


CAISS_STATUS MrptProc::loadDatas(const char* dataPath) {
    CAISS_FUNCTION_BEGIN
    CAISS_ASSERT_NOT_NULL(dataPath)

    std::ifstream inFile(dataPath);
    if (!inFile) {
        return CAISS_RET_PATH;
    }

    unsigned int index = 0;
    std::string line;

    MrptModelHead *headPtr = MrptProc::mrpt_model_head_.get();
    CAISS_ASSERT_NOT_NULL(headPtr)
    while (getline(inFile, line)) {
        if (0 == line.length()) {
            continue;
        }

        CaissDataNode dataNode;
        ret = RapidJsonProc::parseInputData(line.data(), dataNode);
        if (CAISS_RET_OK != ret) {
            break;
        }

        std::vector<CAISS_FLOAT> &node = dataNode.node;
        ret = normalizeNode(node, this->dim_);
        if (CAISS_RET_OK != ret) {
            break;
        }

        // 将信息插入
        ret = processIndexLabel(BIMAP_INSERT, index, dataNode.label);
        if (CAISS_RET_OK != ret) {
            break;
        }
        index++;

        headPtr->modelData.insert(headPtr->modelData.end(),
                                  node.begin(), node.begin() + node.size());
    }

    inFile.close();
    CAISS_FUNCTION_CHECK_STATUS

    CAISS_FUNCTION_END
}


CAISS_STATUS MrptProc::innerSearchResult(void *info,
                                         const CAISS_SEARCH_TYPE searchType,
                                         const unsigned int topK,
                                         const unsigned int filterEditDistance) {
    CAISS_FUNCTION_BEGIN

    CAISS_ASSERT_NOT_NULL(info)
    auto ptr = MrptProc::mrpt_algo_.get();
    CAISS_ASSERT_NOT_NULL(ptr)

    MrptModelHead *modelHead = MrptProc::mrpt_model_head_.get();
    CAISS_ASSERT_NOT_NULL(modelHead)

    ALOG_WORD2VEC_MAP word2VecMap;
    word2VecMap.reserve(8);    // 先分配若干个节点信息

    CAISS_FLOAT_ARRAY vec;    // 向量查询的时候，使用的数据
    vec.reserve(this->dim_);

    switch (searchType) {
        case CAISS_SEARCH_QUERY:
        case CAISS_LOOP_QUERY: {    // 如果传入的是query信息的话
            vec.assign((CAISS_FLOAT *)info, (CAISS_FLOAT *)info + dim_);
            ret = normalizeNode(vec, this->dim_);    // 前面将信息转成query的形式
            CAISS_FUNCTION_CHECK_STATUS

            word2VecMap[QUERY_VIA_ARRAY] = vec;
            break;
        }
        case CAISS_SEARCH_WORD:
        case CAISS_LOOP_WORD: {
            // 如果是传入的是词语信息，根据词的位置
            std::set<string> strSet;    // 存放切分后的单词
            std::string inputInfo = (char *)info;
            boost::algorithm::to_lower(inputInfo);    // 全部转成小写在做判断
            boost::split(strSet, inputInfo, boost::is_any_of(CAISS_SEPARATOR),boost::token_compress_off);    // 空字符不会被推入向量中

            for (std::string str : strSet) {
                unsigned int index = 0;
                ret = processIndexLabel(BIMAP_FIND_LABEL, index, str);
                if (ret < CAISS_RET_OK) {
                    // 如果报错，直接返回
                    break;
                } else if (ret == CAISS_RET_OK) {
                    // 如果找到，则直接获取向量信息
                    vec.assign(modelHead->modelData.begin() + index * this->dim_,
                               modelHead->modelData.begin() + (index + 1) * this->dim_);
                    word2VecMap[str] = vec;
                } else {
                    // 如果没找到，也没报错，则赋空值，继续查询
                    word2VecMap[str] = vec;
                    ret = CAISS_RET_OK;    // 然后当做ok
                }
            }
        }
            break;
        default:
            ret = CAISS_RET_PARAM;
            break;
    }

    CAISS_FUNCTION_CHECK_STATUS
    ALOG_WORD2RESULT_MAP word2ResultMap;

    for (const auto &word2vec : word2VecMap) {
        ALOG_RET_TYPE&& result = this->lru_cache_.get(word2vec.first);
        if (isWordSearchType(searchType) && !result.empty()) {
            // 如果是查询词语的模式，并且缓存中找到了，就不要过滤了，直接当做结果信息
            word2ResultMap[word2vec.first] = result;
        } else {
            auto *query = (CAISS_FLOAT *)word2vec.second.data();
            if (query) {

                int querySize = (int)topK * 7;
                std::vector<int> indexVec;    // 表示位置的数组
                CAISS_FLOAT_ARRAY distVec;
                indexVec.resize(querySize);
                distVec.resize(querySize);

                /* threshold 值越小，准确度越高，查询时间越慢
                 * 使用默认值，确保能够查询到
                 * */
                int threshold = 0;
                ret = ptr->query((const float *)query, querySize, threshold, indexVec.data(), distVec.data());
                CAISS_FUNCTION_CHECK_STATUS

                for (unsigned int i = 0; i < querySize; ++i) {
                    if (-1 == indexVec[i] || -1 == distVec[i]) {
                        break;    // mrpt底层异常值是-1。这样写，说明底层传出来几个，就用几个
                    }
                    result.push(std::make_pair(distVec[i], indexVec[i]));
                }

                ret = filterByRules((void *)word2vec.first.c_str(), searchType,
                                    result, topK, filterEditDistance,
                                    MrptProc::index_label_lookup_);
                CAISS_FUNCTION_CHECK_STATUS

                if (isWordSearchType(searchType)) {
                    this->lru_cache_.put(std::string(word2vec.first), result);
                }
            }
        }

        word2ResultMap[word2vec.first] = result;
    }

    ret = buildResult(topK, searchType, word2ResultMap);
    CAISS_FUNCTION_CHECK_STATUS

    CAISS_FUNCTION_END
}


CAISS_STATUS MrptProc::loadModel() {
    CAISS_FUNCTION_BEGIN

    ret = loadModelHead();
    CAISS_FUNCTION_CHECK_STATUS

    MrptModelHead *headPtr = MrptProc::mrpt_model_head_.get();
    CAISS_ASSERT_NOT_NULL(headPtr)

    if (headPtr->modelTag != MRPT_MODEL_TAG) {
        // mrpt 模型文件头校验
        return CAISS_RET_MODEL_DATA;
    }

    unsigned int index = 0;
    for (auto &label : headPtr->labels) {
        // 插入在bimap中插入单词信息
        ret = processIndexLabel(BIMAP_INSERT, index, label);
        CAISS_FUNCTION_CHECK_STATUS
        index++;
    }

    ret = MrptProc::mrpt_algo_.create(headPtr->modelData.data(),
                                      headPtr->dim, headPtr->curSize);
    CAISS_FUNCTION_CHECK_STATUS

    auto ptr = MrptProc::mrpt_algo_.get();
    CAISS_ASSERT_NOT_NULL(ptr)
    bool result = ptr->load(model_path_.c_str(), headPtr->calcSkipSize());
    if (!result) {
        return CAISS_RET_MODEL_DATA;    // 已经校验过path不为空了，如果读取出问题，就是data的问题了
    }

    CAISS_FUNCTION_END
}


/*
* 读取模型头信息，为重新生成模型做准备
* 注：这里仅读取mrpt模型头信息，做校验使用。
* */
CAISS_STATUS MrptProc::loadModelHead() {
    CAISS_FUNCTION_BEGIN

    FILE *fd = nullptr;
    if ((fd = fopen(model_path_.c_str(), "rb")) == nullptr) {
        return CAISS_RET_PATH;
    }

    fseek(fd, 0, SEEK_SET);    // 移动至开头

    MrptModelHead *modelHead = MrptProc::mrpt_model_head_.get();
    CAISS_ASSERT_NOT_NULL(modelHead)

    fread(&(modelHead->modelTag), sizeof(int), 1, fd);
    fread(&(modelHead->maxDataSize), sizeof(int), 1, fd);
    fread(&(modelHead->dim), sizeof(int), 1, fd);
    fread(&(modelHead->maxIndexSize), sizeof(int), 1, fd);
    fread(&(modelHead->curSize), sizeof(int), 1, fd);

    int len = modelHead->curSize * modelHead->dim;
    modelHead->modelData.resize(len);
    fread(modelHead->modelData.data(), sizeof(CAISS_FLOAT), len, fd);

    char* buf = new char[modelHead->maxIndexSize];
    memset(buf, 0, modelHead->maxIndexSize);
    for (int i = 0; i < modelHead->curSize; i++) {
        memset(buf, 0, modelHead->maxIndexSize);
        fread(buf, modelHead->maxIndexSize, 1, fd);
        modelHead->labels.emplace_back(buf);
    }
    delete []buf;

    fclose(fd);
    CAISS_FUNCTION_END
}


CAISS_STATUS MrptProc::buildResult(unsigned int topK,
                                   CAISS_SEARCH_TYPE searchType,
                                   const ALOG_WORD2RESULT_MAP &word2ResultMap) {
    CAISS_FUNCTION_BEGIN

    auto ptr = MrptProc::mrpt_algo_.get();
    CAISS_ASSERT_NOT_NULL(ptr)

    auto modelHead = MrptProc::mrpt_model_head_.get();
    CAISS_ASSERT_NOT_NULL(modelHead);

    for (const auto &word2Result : word2ResultMap) {
        std::list<CaissResultDetail> detailsList;
        auto result = word2Result.second;

        while (!result.empty()) {
            CaissResultDetail detail;
            auto cur = result.top();
            result.pop();

            detail.distance = cur.first;
            detail.index = cur.second;
            processIndexLabel(BIMAP_FIND_INDEX, detail.index, detail.label);

            detail.node.resize(this->dim_);
            // 从head中获取信息
            detail.node.assign(modelHead->modelData.begin() + detail.index * this->dim_,
                               modelHead->modelData.begin() + (detail.index + 1) * this->dim_);

            detailsList.push_front(detail);
        }

        word_details_map_[word2Result.first] = detailsList;
    }

    std::string type = isAnnSearchType(searchType) ? "ann_search" : "force_loop";
    ret = RapidJsonProc::buildSearchResult(word_details_map_, this->distance_type_, type, topK, this->result_);
    CAISS_FUNCTION_CHECK_STATUS

    CAISS_FUNCTION_END
}