//
// Created by Chunel on 2020/5/23.
// hnsw算法的封装层，对外暴漏的算法使用接口
//

#include <fstream>
#include <queue>
#include "HnswProc.h"

using namespace std;


HnswProc::HnswProc() {
    resetHnswProcMember();
}

HnswProc::~HnswProc() {
    resetHnswProcMember();
}

/************************ 以下是重写的算法基类接口内容 ************************/
ANN_RET_TYPE HnswProc::init(const ANN_MODE mode, const unsigned int dim, const char *modelPath, const unsigned int exLen) {
    ANN_ASSERT_NOT_NULL(modelPath);

    this->dim_ = dim;
    this->cur_mode_ = mode;
    this->model_path_ = string(modelPath);   // 如果是train模式，则是需要保存到这里；如果process模式，则是读取模型

    if (model_path_.find(MODEL_SUFFIX) != model_path_.length() - string(MODEL_SUFFIX).length()) {
        // 如果传入的路径，不是以.ann结尾的，则添加.ann作为文件格式
        this->model_path_ += MODEL_SUFFIX;
    }

    ANN_FUNCTION_END
}

ANN_RET_TYPE HnswProc::deinit() {
    ANN_FUNCTION_END
}

ANN_RET_TYPE HnswProc::train(const char* dataPath, const unsigned int maxDataSize, const ANN_BOOL normalize, const float precision, const unsigned int fastRank,
                             const unsigned int realRank, const unsigned int step, const unsigned int maxEpoch, const unsigned int showSpan) {
    ANN_ASSERT_NOT_NULL(dataPath)

    ANN_FUNCTION_BEGIN
    ANN_CHECK_MODE_ENABLE(ANN_MODE_TRAIN, this->cur_mode_)

    this->l2s_ptr_ = new L2Space(this->dim_);
    this->hnsw_alg_ptr_ = new HierarchicalNSW<ANN_FLOAT>(l2s_ptr_, 100, normalize);    // todo，训练的时候，用这个构造函数，后面还有其他参数
    this->json_proc_ = new RapidJsonProc();
    this->normalize_ = normalize;   // 外部设定是否需要做归一化

    std::vector<ANN_VECTOR_FLOAT> datas;
    ret = loadDatas(dataPath, datas);
    ANN_FUNCTION_CHECK_STATUS

    ret = trainModel(datas);
    ANN_FUNCTION_CHECK_STATUS

    ANN_FUNCTION_END
}

ANN_RET_TYPE HnswProc::search(const ANN_FLOAT *query, const unsigned int topK, const ANN_SEARCH_TYPE searchType) {
    ANN_FUNCTION_BEGIN

    std::priority_queue<std::pair<ANN_FLOAT, labeltype>> result = hnsw_alg_ptr_->searchKnn((void *)query, topK);

    std::vector<unsigned int> predIndex;
    while (!result.empty()) {
        // 把预测到的结果，pred_dist中去
        auto index = (unsigned int)result.top().second;
        result.pop();
        predIndex.push_back(index);
    }

    ANN_FUNCTION_END
}

ANN_RET_TYPE HnswProc::insert(const ANN_FLOAT *node, const char *label, const ANN_INSERT_TYPE insertType) {
    ANN_FUNCTION_END
}

ANN_RET_TYPE HnswProc::save(char *modePah) {
    ANN_FUNCTION_BEGIN


    ANN_FUNCTION_END
}

ANN_RET_TYPE HnswProc::getResultSize(unsigned int &size) {
    ANN_FUNCTION_END
}

ANN_RET_TYPE HnswProc::getResult(char *result, unsigned int size) {
    ANN_FUNCTION_END
}

ANN_RET_TYPE HnswProc::resetHnswProcMember() {
    ANN_FUNCTION_BEGIN

    ANN_INIT_NULLPTR(this->hnsw_alg_ptr_)
    ANN_INIT_NULLPTR(this->l2s_ptr_)
    ANN_INIT_NULLPTR(this->json_proc_)

    ANN_FUNCTION_END
};
/******************************************************/


/************************ 以下是本Proc类内部函数 ************************/


/**
 * 读取文件中信息，并存至datas中
 * @param datas
 * @return
 */
ANN_RET_TYPE HnswProc::loadDatas(const char *dataPath, std::vector<ANN_VECTOR_FLOAT> &datas) {
    ANN_ASSERT_NOT_NULL(this->json_proc_)
    ANN_ASSERT_NOT_NULL(dataPath);

    ANN_FUNCTION_BEGIN
    datas.clear();

    std::ifstream in(dataPath);
    if (!in) {
        return ANN_RET_PATH;
    }

    std::string line;
    while (getline(in, line)) {
        if (0 == line.length()) {
            continue;    // 排除空格的情况
        }

        std::vector<ANN_FLOAT> node;
        ret = json_proc_->parseInputData(line.data(), node);
        ANN_FUNCTION_CHECK_STATUS

        datas.push_back(node);
    }

    ANN_FUNCTION_END
}

ANN_RET_TYPE HnswProc::trainModel(vector<ANN_VECTOR_FLOAT> datas) {
    ANN_ASSERT_NOT_NULL(this->hnsw_alg_ptr_)

    ANN_FUNCTION_BEGIN

    unsigned int size = datas.size();
    for (unsigned int i = 0; i < size; i++) {
        ret = normalizeNode((ANN_FLOAT *)datas[i].data());    // 在normalizeNode函数内部，判断是否需要归一化
        ANN_FUNCTION_CHECK_STATUS
        this->hnsw_alg_ptr_->addPoint((void *)datas[i].data(), i);
    }

    this->hnsw_alg_ptr_->saveIndex(std::string(this->model_path_));
    ANN_FUNCTION_END
}

