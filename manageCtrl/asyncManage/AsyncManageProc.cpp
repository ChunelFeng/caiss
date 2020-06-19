//
// Created by Chunel on 2020/6/20.
//

#include "AsyncManageProc.h"

AsyncManageProc::AsyncManageProc(unsigned int maxSize, CAISS_ALGO_TYPE algoType) : manageProc(maxSize, algoType) {
    this->pool_ptr_  = new ThreadPool(maxSize);
}

AsyncManageProc::~AsyncManageProc() {
    CAISS_DELETE_PTR(this->pool_ptr_);
}

CAISS_RET_TYPE AsyncManageProc::createHandle(void **handle) {
    CAISS_FUNCTION_BEGIN

    CAISS_FUNCTION_END
}

CAISS_RET_TYPE AsyncManageProc::destroyHandle(void *handle) {
    CAISS_FUNCTION_END
}

CAISS_RET_TYPE AsyncManageProc::init(void *handle, CAISS_MODE mode, CAISS_DISTANCE_TYPE distanceType, unsigned int dim,
                                     const char *modelPath, CAISS_DIST_FUNC distFunc) {
    CAISS_FUNCTION_END
}

CAISS_RET_TYPE
AsyncManageProc::train(void *handle, const char *dataPath, unsigned int maxDataSize, CAISS_BOOL normalize,
                       unsigned int maxIndexSize, float precision, unsigned int fastRank, unsigned int realRank,
                       unsigned int step, unsigned int maxEpoch, unsigned int showSpan) {
    CAISS_FUNCTION_END
}

CAISS_RET_TYPE AsyncManageProc::search(void *handle, void *info, CAISS_SEARCH_TYPE searchType, unsigned int topK) {
    CAISS_FUNCTION_END
}

CAISS_RET_TYPE AsyncManageProc::getResultSize(void *handle, unsigned int &size) {
    CAISS_FUNCTION_END
}

CAISS_RET_TYPE AsyncManageProc::getResult(void *handle, char *result, unsigned int size) {
    CAISS_FUNCTION_END
}

CAISS_RET_TYPE
AsyncManageProc::insert(void *handle, CAISS_FLOAT *node, const char *label, CAISS_INSERT_TYPE insertType) {
    CAISS_FUNCTION_END
}

CAISS_RET_TYPE AsyncManageProc::save(void *handle, const char *modelPath) {
    CAISS_FUNCTION_END
}


