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


ANN_RET_TYPE HnswProc::search(ANN_FLOAT *query, const unsigned int topK, const ANN_SEARCH_TYPE searchType) {
    ANN_FUNCTION_BEGIN

    ANN_ASSERT_NOT_NULL(query)
    ANN_CHECK_MODE_ENABLE(ANN_MODE_PROCESS)

    ret = normalizeNode(query, this->dim_);
    ANN_FUNCTION_CHECK_STATUS

    this->result_.clear();
    std::priority_queue<std::pair<ANN_FLOAT, labeltype>> result = HnswProc::getHnswSingleton()->searchKnn((void *)query, topK);

    std::list<unsigned int> predIndex;
    while (!result.empty()) {
        // 把预测到的结果，pred_dist中去
        auto index = (unsigned int)result.top().second;
        result.pop();
        predIndex.push_front(index);
    }

    ret = buildResult(query, predIndex);
    ANN_FUNCTION_CHECK_STATUS

    ANN_FUNCTION_END
}


ANN_RET_TYPE HnswProc::insert(ANN_FLOAT *node, const char *index, ANN_INSERT_TYPE insertType) {
    ANN_FUNCTION_BEGIN
    ANN_ASSERT_NOT_NULL(node)
    ANN_ASSERT_NOT_NULL(index)

    ANN_CHECK_MODE_ENABLE(ANN_MODE_PROCESS)

    unsigned int curCount = HnswProc::getHnswSingleton()->cur_element_count_;
    if (HnswProc::getHnswSingleton()->max_elements_ <= curCount) {
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
    ANN_ASSERT_NOT_NULL(HnswProc::getHnswSingleton())

    std::string path;
    if (nullptr == modelPath) {
        path = this->model_path_;    // 如果传入的值为空，则保存当前的模型
    } else {
        path = isAnnSuffix(modelPath) ? string(modelPath) : (string(modelPath) + MODEL_SUFFIX);
    }

    remove(path.c_str());    // 如果有的话，就删除
    HnswProc::getHnswSingleton()->saveIndex(path);
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
    ANN_ASSERT_NOT_NULL(HnswProc::getHnswSingleton())

    unsigned int size = datas.size();
    for (unsigned int i = 0; i < size; i++) {
        ret = normalizeNode(datas[i].node.data(), this->dim_);    // 在normalizeNode函数内部，判断是否需要归一化
        ANN_FUNCTION_CHECK_STATUS
        HnswProc::getHnswSingleton()->addPoint((void *)datas[i].node.data(), i, (char *)datas[i].index.c_str());    // addPoint这里加入的i，算是位置信息
    }

    HnswProc::getHnswSingleton()->saveIndex(std::string(this->model_path_));
    ANN_FUNCTION_END
}


ANN_RET_TYPE HnswProc::buildResult(const ANN_FLOAT *query, const std::list<unsigned int> &predIndex) {
    ANN_FUNCTION_BEGIN
    ANN_ASSERT_NOT_NULL(HnswProc::getHnswSingleton())
    ANN_ASSERT_NOT_NULL(HnswProc::getHnswSingleton()->fstdistfunc_)
    ANN_ASSERT_NOT_NULL(HnswProc::getHnswSingleton()->dist_func_param_)
    ANN_ASSERT_NOT_NULL(query)

    std::vector<AnnResultDetail> details;

    for (unsigned int i : predIndex) {
        AnnResultDetail detail;
        detail.node = HnswProc::getHnswSingleton()->getDataByLabel<ANN_FLOAT>(i);
        detail.distance = HnswProc::getHnswSingleton()->fstdistfunc_((void *)detail.node.data(), (void *)query, HnswProc::getHnswSingleton()->dist_func_param_);
        detail.index = i;
        details.push_back(detail);
    }

    ret = RapidJsonProc::buildSearchResult(details, this->distance_type_, this->result_);
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


ANN_RET_TYPE HnswProc::insertByOverwrite(ANN_FLOAT *node, unsigned int curCount, const char *index) {
    ANN_FUNCTION_BEGIN

    ANN_ASSERT_NOT_NULL(node)    // 传入的信息，已经是normalize后的信息了
    ANN_ASSERT_NOT_NULL(index)
    auto ptr = HnswProc::getHnswSingleton();
    ANN_ASSERT_NOT_NULL(ptr);

    unsigned int label = curCount + 1;

    ret = ptr->addPoint((void *)node, label, index);
    ANN_FUNCTION_CHECK_STATUS

    ANN_FUNCTION_END
}


ANN_RET_TYPE HnswProc::insertByDiscard(ANN_FLOAT *node, unsigned int curCount, const char *index) {
    ANN_FUNCTION_BEGIN

    ANN_ASSERT_NOT_NULL(node)
    ANN_ASSERT_NOT_NULL(index)
    auto ptr = HnswProc::getHnswSingleton();
    ANN_ASSERT_NOT_NULL(ptr)

    unsigned int label = curCount + 1;
    ret = ptr->checkAddEnable(label, index);
    if (1 == ret) {
        return ANN_RET_OK;    // 特殊逻辑，如果该label，已经被用了，则无需任何操作
    }
    ANN_FUNCTION_CHECK_STATUS

    ret = ptr->addPoint(node, label, index);
    ANN_FUNCTION_CHECK_STATUS

    ANN_FUNCTION_END
}


