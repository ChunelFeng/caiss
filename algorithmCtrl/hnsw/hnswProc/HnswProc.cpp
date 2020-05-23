//
// Created by Chunel on 2020/5/23.
// hnsw算法的封装层，对外暴漏的算法使用接口
//

#include <fstream>
#include "HnswProc.h"


HnswProc::HnswProc() {
    resetMemberViriables();
}

HnswProc::~HnswProc() {
    resetMemberViriables();
}

/************************ 以下是重写的算法基类接口内容 ************************/
ANN_RET_TYPE HnswProc::init(ANN_MODE mode, unsigned int dim, char *modelPath, unsigned int exLen) {
    ANN_ASSERT_NOT_NULL(modelPath);

    this->dim_ = dim;
    this->cur_mode_ = mode;
    this->model_path_ptr_ = modelPath;   // 如果是train模式，则是需要保存到这里；如果process模式，则是读取模型

    ANN_FUNCTION_END
}

ANN_RET_TYPE HnswProc::deinit() {
    ANN_FUNCTION_END
}

ANN_RET_TYPE HnswProc::train(char *dataPath, unsigned int maxDataSize, ANN_BOOL normalize, float precision, unsigned int fastRank,
                unsigned int realRank, unsigned int step, unsigned int maxEpoch, unsigned int showSpan) {
    ANN_ASSERT_NOT_NULL(dataPath)

    ANN_FUNCTION_BEGIN
    ANN_CHECK_MODE_ENABLE(ANN_MODE_TRAIN, this->cur_mode_)


    this->l2s_ptr_ = new L2Space(this->dim_);
    this->hnsw_alg_ptr_ = new HierarchicalNSW<ANN_FLOAT>(l2s_ptr_, std::string(model_path_ptr_));
    this->json_proc_ = new RapidJsonProc();
    this->normalize_ = normalize;

    std::vector<ANN_VECTOR_FLOAT> datas;
    ret = loadDatas(datas);
    ANN_FUNCTION_CHECK_STATUS

    ret = trainModel(datas);
    ANN_FUNCTION_CHECK_STATUS

    ANN_FUNCTION_END
}

ANN_RET_TYPE HnswProc::search(ANN_FLOAT *query, unsigned int topK, ANN_SEARCH_TYPE searchType) {
    ANN_FUNCTION_END
}

ANN_RET_TYPE HnswProc::insert(ANN_FLOAT *node, char *label, ANN_INSERT_TYPE insertType) {
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

ANN_RET_TYPE HnswProc::resetMemberViriables() {
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
ANN_RET_TYPE HnswProc::loadDatas(std::vector<ANN_VECTOR_FLOAT> &datas) {
    ANN_ASSERT_NOT_NULL(this->json_proc_)
    ANN_ASSERT_NOT_NULL(this->model_path_ptr_)

    ANN_FUNCTION_BEGIN

    std::ifstream in(this->model_path_ptr_);
    if (!in) {
        return ANN_RET_PATH;
    }

    std::string line;
    while (getline(in, line)) {
        std::vector<ANN_FLOAT> node;
        ret = json_proc_->parseInputData(line.data(), node);
        ANN_FUNCTION_CHECK_STATUS
        datas.push_back(node);
    }

    ANN_FUNCTION_END
}

ANN_RET_TYPE HnswProc::trainModel(const std::vector<ANN_VECTOR_FLOAT> datas) {
    ANN_ASSERT_NOT_NULL(this->hnsw_alg_ptr_)
    ANN_ASSERT_NOT_NULL(this->model_path_ptr_)

    ANN_FUNCTION_BEGIN

    unsigned int size = datas.size();
    for (unsigned int i = 0; i < size; i++) {
        this->hnsw_alg_ptr_->addPoint((void *)datas[i].data(), i);
    }

    this->hnsw_alg_ptr_->saveIndex(std::string(this->model_path_ptr_));
    ANN_FUNCTION_END
}
