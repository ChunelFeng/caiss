//
// Created by Chunel on 2020/5/23.
// hnsw算法的封装层，对外暴漏的算法使用接口
//

#include "HnswProc.h"


HnswProc::HnswProc() {
    ANN_INIT_NULLPTR(this->hnsw_alg_ptr_)
    ANN_INIT_NULLPTR(this->l2s_ptr_)
}

HnswProc::~HnswProc() {
    ANN_DELETE_PTR(this->hnsw_alg_ptr_)
    ANN_DELETE_PTR(this->l2s_ptr_)
}

/************************ 以下是重写的算法基类接口内容 ************************/
ANN_RET_TYPE HnswProc::init(ANN_MODE mode, unsigned int dim, char *modelPath, unsigned int exLen) {
    ANN_ASSERT_NOT_NULL(modelPath);

    this->dim_ = dim;
    this->cur_mode_ = mode;
    this->model_path_ = std::string(modelPath);   // 如果是train模式，则是需要保存到这里；如果process模式，则是读取模型

    return ANN_RET_OK;
}

ANN_RET_TYPE HnswProc::deinit() {
    return ANN_RET_OK;
}

ANN_RET_TYPE HnswProc::train(char *dataPath, unsigned int maxDataSize, ANN_BOOL normalize, float precision, unsigned int fastRank,
                unsigned int realRank, unsigned int step, unsigned int maxEpoch, unsigned int showSpan) {
    ANN_ASSERT_NOT_NULL(dataPath)

    l2s_ptr_ = new L2Space(this->dim_);
    hnsw_alg_ptr_ = new HierarchicalNSW<ANN_FLOAT>(l2s_ptr_, model_path_);
    this->normalize_ = normalize;


    return ANN_RET_OK;
}

ANN_RET_TYPE HnswProc::search(ANN_FLOAT *query, unsigned int topK, ANN_SEARCH_TYPE searchType) {
    return ANN_RET_OK;
}

ANN_RET_TYPE HnswProc::insert(ANN_FLOAT *node, char *label, ANN_INSERT_TYPE insertType) {
    return ANN_RET_OK;
}

ANN_RET_TYPE HnswProc::save(char *modePah) {
    return ANN_RET_OK;
}

ANN_RET_TYPE HnswProc::getResultSize(unsigned int &size) {
    return ANN_RET_OK;
}

ANN_RET_TYPE HnswProc::getResult(char *result, unsigned int size) {
    return ANN_RET_OK;
}


/******************************************************/


/************************ 以下是本Proc类内部函数 ************************/
