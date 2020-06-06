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
HierarchicalNSW<ANN_FLOAT>*  HnswProc::hnsw_alg_ptr_ = nullptr;
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
ANN_RET_TYPE
HnswProc::init(const ANN_MODE mode, const ANN_DISTANCE_TYPE distanceType, const unsigned int dim, const char *modelPath,
               const unsigned int exLen) {
    ANN_FUNCTION_BEGIN
    ANN_ASSERT_NOT_NULL(modelPath);

    reset();    // 清空所有数据信息

    this->dim_ = dim;
    this->cur_mode_ = mode;
    // 如果是train模式，则是需要保存到这里；如果process模式，则是读取模型
    this->model_path_ = isAnnSuffix(modelPath) ? (string(modelPath)) : (string(modelPath) + MODEL_SUFFIX);
    this->distance_type_ = distanceType;
    createDistancePtr();

    if (this->cur_mode_ == ANN_MODE_PROCESS) {
        ret = loadModel(modelPath);    // 如果是处理模式的话，则读取模型内容信息
        ANN_FUNCTION_CHECK_STATUS
    }

    ANN_FUNCTION_END
}


ANN_RET_TYPE HnswProc::reset() {
    ANN_FUNCTION_BEGIN

    ANN_DELETE_PTR(distance_ptr_)
    this->dim_ = 0;
    this->cur_mode_ = ANN_MODE_DEFAULT;
    this->normalize_ = 0;
    this->result_.clear();

    ANN_FUNCTION_END
}


ANN_RET_TYPE HnswProc::train(const char* dataPath, const unsigned int maxDataSize, const ANN_BOOL normalize, const float precision,
                             const unsigned int fastRank, const unsigned int realRank,
                             const unsigned int step, const unsigned int maxEpoch, const unsigned int showSpan) {
    ANN_FUNCTION_BEGIN
    ANN_ASSERT_NOT_NULL(dataPath)
    ANN_ASSERT_NOT_NULL(this->distance_ptr_)
    ANN_CHECK_MODE_ENABLE(ANN_MODE_TRAIN)

    this->normalize_ = normalize;

    HnswProc::createHnswSingleton(this->distance_ptr_, maxDataSize, normalize);

    std::vector<AnnDataNode> datas;
    datas.reserve(maxDataSize);    // 提前分配好内存信息
    ret = loadDatas(dataPath, datas);
    ANN_FUNCTION_CHECK_STATUS

    ret = trainModel(datas);
    ANN_FUNCTION_CHECK_STATUS

    ANN_FUNCTION_END
}


ANN_RET_TYPE HnswProc::search(void *info, ANN_SEARCH_TYPE searchType, const unsigned int topK) {
    ANN_FUNCTION_BEGIN

    ANN_ASSERT_NOT_NULL(info)
    auto ptr = HnswProc::getHnswSingleton();
    ANN_ASSERT_NOT_NULL(ptr)
    ANN_CHECK_MODE_ENABLE(ANN_MODE_PROCESS)

    ANN_FLOAT *query = nullptr;
    switch (searchType) {
        case ANN_SEARCH_QUERY: {
            query = (ANN_FLOAT *)info;    // 如果传入的是query信息的话
            break;
        }
        case ANN_SEARCH_WORD: {
            int label = ptr->findWordLabel((const char *)info);
            if (-1 != label) {
                query = ptr->getDataByLabel<ANN_FLOAT>(label).data();    // 找到word的情况
            } else {
                ret = ANN_RET_NO_WORD;    // 没有找到word的情况
            }
            break;
        }
        default:
            ret = ANN_RET_PARAM;
            break;
    }

    ANN_FUNCTION_CHECK_STATUS

    ret = normalizeNode(query, this->dim_);
    ANN_FUNCTION_CHECK_STATUS

    this->result_.clear();
    std::priority_queue<std::pair<ANN_FLOAT, labeltype>> result = ptr->searchKnn((void *)query, topK);

    ret = buildResult(query, result);
    ANN_FUNCTION_CHECK_STATUS

    ANN_FUNCTION_END
}


//ANN_RET_TYPE HnswProc::search(const char *word, const unsigned int topK, ANN_SEARCH_TYPE searchType) {
//    ANN_FUNCTION_BEGIN
//
//    ANN_ASSERT_NOT_NULL(word)
//    auto ptr = HnswProc::getHnswSingleton();
//    ANN_ASSERT_NOT_NULL(ptr)
//
//    ANN_CHECK_MODE_ENABLE(ANN_MODE_PROCESS)
//    this->result_.clear();
//    this->result_words_.clear();
//
//
//
//    int label = ptr->findWordLabel(word);
//    if (-1 != label) {
//        // 如果找到了
//        ANN_VECTOR_FLOAT node = ptr->getDataByLabel<ANN_FLOAT>(label);
//        ret = search(node.data(), topK, searchType);    // 根据这个word对应的label信息，去生成其相近的信息
//    } else {
//        cout << "sorry, we find nothing for the word : " << word << endl;
//        ret = ANN_RET_NO_WORD;
//    }
//    ANN_FUNCTION_CHECK_STATUS
//
//    ANN_FUNCTION_END
//}


ANN_RET_TYPE HnswProc::insert(ANN_FLOAT *node, const char *index, ANN_INSERT_TYPE insertType) {
    ANN_FUNCTION_BEGIN
    ANN_ASSERT_NOT_NULL(node)
    ANN_ASSERT_NOT_NULL(index)
    auto ptr = HnswProc::getHnswSingleton();
    ANN_ASSERT_NOT_NULL(ptr)

    ANN_CHECK_MODE_ENABLE(ANN_MODE_PROCESS)

    unsigned int curCount = ptr->cur_element_count_;
    if (curCount >= ptr->max_elements_) {
        return ANN_RET_MODEL_SIZE;    // 超过模型的最大尺寸了
    }

    ret = normalizeNode(node, this->dim_);
    ANN_FUNCTION_CHECK_STATUS

    switch (insertType) {
        case ANN_INSERT_OVERWRITE:
            ret = insertByOverwrite(node, curCount, index);
            break;
        case ANN_INSERT_DISCARD:
            ret = insertByDiscard(node, curCount, index);
            break;
        default:
            ret = ANN_RET_PARAM;
            break;
    }

    ANN_FUNCTION_CHECK_STATUS

    ANN_FUNCTION_END
}


ANN_RET_TYPE HnswProc::save(const char *modelPath) {
    ANN_FUNCTION_BEGIN
    auto ptr = HnswProc::getHnswSingleton();
    ANN_ASSERT_NOT_NULL(ptr)

    std::string path;
    if (nullptr == modelPath) {
        path = this->model_path_;    // 如果传入的值为空，则保存当前的模型
    } else {
        path = isAnnSuffix(modelPath) ? string(modelPath) : (string(modelPath) + MODEL_SUFFIX);
    }

    remove(path.c_str());    // 如果有的话，就删除
    ptr->saveIndex(path);

    ANN_FUNCTION_END
}


ANN_RET_TYPE HnswProc::getResultSize(unsigned int &size) {
    ANN_FUNCTION_BEGIN
    ANN_CHECK_MODE_ENABLE(ANN_MODE_PROCESS)

    size = this->result_.size();

    ANN_FUNCTION_END
}


ANN_RET_TYPE HnswProc::getResult(char *result, unsigned int size) {
    ANN_FUNCTION_BEGIN
    ANN_ASSERT_NOT_NULL(result)
    ANN_CHECK_MODE_ENABLE(ANN_MODE_PROCESS)

    memset(result, 0, size);
    memcpy(result, this->result_.data(), this->result_.size());

    ANN_FUNCTION_END
}


ANN_RET_TYPE HnswProc::ignore(const char *label) {
    ANN_FUNCTION_BEGIN

    ANN_ASSERT_NOT_NULL(label)
    // todo 逻辑待实现

    ANN_FUNCTION_END
}


/************************ 以下是本Proc类内部函数 ************************/
/**
 * 读取文件中信息，并存至datas中
 * @param datas
 * @return
 */
ANN_RET_TYPE HnswProc::loadDatas(const char *dataPath, vector<AnnDataNode> &datas) {
    ANN_FUNCTION_BEGIN
    ANN_ASSERT_NOT_NULL(dataPath);

    std::ifstream in(dataPath);
    if (!in) {
        return ANN_RET_PATH;
    }

    std::string line;
    while (getline(in, line)) {
        if (0 == line.length()) {
            continue;    // 排除空格的情况
        }

        AnnDataNode dataNode;
        ret = RapidJsonProc::parseInputData(line.data(), dataNode);
        ANN_FUNCTION_CHECK_STATUS

        datas.push_back(dataNode);
    }

    ANN_FUNCTION_END
}


ANN_RET_TYPE HnswProc::trainModel(vector<AnnDataNode> &datas) {
    ANN_FUNCTION_BEGIN
    auto ptr = HnswProc::getHnswSingleton();
    ANN_ASSERT_NOT_NULL(ptr)

    unsigned int size = datas.size();
    for (unsigned int i = 0; i < size; i++) {
        ret = normalizeNode(datas[i].node.data(), this->dim_);    // 在normalizeNode函数内部，判断是否需要归一化
        ANN_FUNCTION_CHECK_STATUS
        ret = insertByOverwrite(datas[i].node.data(), i, (char *)datas[i].index.c_str());
        ANN_FUNCTION_CHECK_STATUS

        if (i % 1000 == 0) {
            std::cout << "====" << i << "====" << std::endl;
        }
    }

    ptr->saveIndex(std::string(this->model_path_));
    ANN_FUNCTION_END
}


ANN_RET_TYPE HnswProc::buildResult(const ANN_FLOAT *query, std::priority_queue<std::pair<ANN_FLOAT, labeltype>>  &predResult) {
    ANN_FUNCTION_BEGIN
    ANN_ASSERT_NOT_NULL(query)
    auto ptr = HnswProc::getHnswSingleton();
    ANN_ASSERT_NOT_NULL(ptr);

    std::list<AnnResultDetail> detailsList;
    while (!predResult.empty()) {
        AnnResultDetail detail;
        auto cur = predResult.top();
        detail.node = ptr->getDataByLabel<ANN_FLOAT>(cur.second);
        detail.distance = cur.first;
        detail.index = cur.second;
        detail.label = ptr->index_lookup_.left.find(cur.second)->second;
        detailsList.push_front(detail);

        predResult.pop();
    }

    ret = RapidJsonProc::buildSearchResult(detailsList, this->distance_type_, this->result_);
    ANN_FUNCTION_CHECK_STATUS

    ANN_FUNCTION_END
}


ANN_RET_TYPE HnswProc::loadModel(const char *modelPath) {
    ANN_FUNCTION_BEGIN

    ANN_ASSERT_NOT_NULL(modelPath)
    ANN_ASSERT_NOT_NULL(this->distance_ptr_)

    HnswProc::createHnswSingleton(this->distance_ptr_, this->model_path_);    // 读取模型的时候，使用的获取方式
    this->normalize_ = HnswProc::getHnswSingleton()->normalize_;    // 保存模型的时候，会写入是否被标准化的信息

    ANN_FUNCTION_END
}


ANN_RET_TYPE HnswProc::createDistancePtr() {
    ANN_FUNCTION_BEGIN

    ANN_DELETE_PTR(this->distance_ptr_)    // 先删除，确保不会出现重复new的情况
    switch (this->distance_type_) {
        case ANN_DISTANCE_EUC :
            this->distance_ptr_ = new L2Space(this->dim_);
            break;
        case ANN_DISTANCE_INNER:
            this->distance_ptr_ = new InnerProductSpace(this->dim_);
            break;
        case ANN_DISTANCE_EDITION:    // todo 今后需要外部传入距离计算的函数
        default:
            break;
    }

    ANN_FUNCTION_END
}


/**
 * 训练模型的时候，使用的构建方式（static成员函数）
 * @param distance_ptr
 * @param maxDataSize
 * @param normalize
 * @return
 */
ANN_RET_TYPE HnswProc::createHnswSingleton(SpaceInterface<ANN_FLOAT> *distance_ptr, unsigned int maxDataSize, ANN_BOOL normalize) {
    ANN_FUNCTION_BEGIN

    if (nullptr == HnswProc::hnsw_alg_ptr_) {
        HnswProc::lock_.writeLock();
        if (nullptr == HnswProc::hnsw_alg_ptr_) {
            HnswProc::hnsw_alg_ptr_ = new HierarchicalNSW<ANN_FLOAT>(distance_ptr, maxDataSize, normalize);
        }
        HnswProc::lock_.writeUnlock();
    }

    ANN_FUNCTION_END
}

/**
 * 加载模型的时候，使用的构建方式（static成员函数）
 * @param distance_ptr
 * @param modelPath
 * @return
 */
ANN_RET_TYPE HnswProc::createHnswSingleton(SpaceInterface<ANN_FLOAT> *distance_ptr, const std::string &modelPath) {
    ANN_FUNCTION_BEGIN

    if (nullptr == HnswProc::hnsw_alg_ptr_) {
        HnswProc::lock_.writeLock();
        if (nullptr == HnswProc::hnsw_alg_ptr_) {
            // 这里是static函数信息，只能通过传递值下来的方式实现
            HnswProc::hnsw_alg_ptr_ = new HierarchicalNSW<ANN_FLOAT>(distance_ptr, modelPath);
        }
        HnswProc::lock_.writeUnlock();
    }

    ANN_FUNCTION_END
}

HierarchicalNSW<ANN_FLOAT> *HnswProc::getHnswSingleton() {
    return HnswProc::hnsw_alg_ptr_;
}


ANN_RET_TYPE HnswProc::insertByOverwrite(ANN_FLOAT *node, unsigned int label, const char *index) {
    ANN_FUNCTION_BEGIN

    ANN_ASSERT_NOT_NULL(node)    // 传入的信息，已经是normalize后的信息了
    ANN_ASSERT_NOT_NULL(index)
    auto ptr = HnswProc::getHnswSingleton();
    ANN_ASSERT_NOT_NULL(ptr);

    if (-1 == ptr->findWordLabel(index)) {
        // 返回-1，表示没找到对应的信息，如果不存在，则插入内容
        ret = ptr->addPoint(node, label, index);
    } else {
        // 如果被插入过了，则覆盖之前的内容，覆盖的时候，不需要考虑label的值，因为在里面，可以通过index获取
        ret = ptr->overwriteNode(node, index);
    }
    ANN_FUNCTION_CHECK_STATUS

    ANN_FUNCTION_END
}


ANN_RET_TYPE HnswProc::insertByDiscard(ANN_FLOAT *node, unsigned int label, const char *index) {
    ANN_FUNCTION_BEGIN

    ANN_ASSERT_NOT_NULL(node)
    ANN_ASSERT_NOT_NULL(index)
    auto ptr = HnswProc::getHnswSingleton();
    ANN_ASSERT_NOT_NULL(ptr)

    bool bret = ptr->findWordLabel(index);
    if (!bret) {
        // 如果不存在，则直接添加；如果存在，则不进入此逻辑，直接返回
        ret = ptr->addPoint(node, label, index);
        ANN_FUNCTION_CHECK_STATUS
    }

    ANN_FUNCTION_END
}


