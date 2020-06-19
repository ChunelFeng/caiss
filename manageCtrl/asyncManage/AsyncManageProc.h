//
// Created by Chunel on 2020/6/20.
// 开启异步调用流程
//

#ifndef CAISS_ASYNCMANAGEPROC_H
#define CAISS_ASYNCMANAGEPROC_H

#include "../manageProc.h"

class AsyncManageProc : public manageProc {
public:
    explicit AsyncManageProc(unsigned int maxSize, CAISS_ALGO_TYPE algoType);
    ~AsyncManageProc() override ;

    CAISS_RET_TYPE createHandle(void **handle) override ;    // 生成处理句柄
    CAISS_RET_TYPE destroyHandle(void *handle) override ;

    CAISS_RET_TYPE init(void *handle, CAISS_MODE mode, CAISS_DISTANCE_TYPE distanceType,
                        unsigned int dim, const char *modelPath, CAISS_DIST_FUNC distFunc) override ;

    CAISS_RET_TYPE train(void *handle, const char *dataPath, unsigned int maxDataSize, CAISS_BOOL normalize,
                         unsigned int maxIndexSize, float precision, unsigned int fastRank,
                         unsigned int realRank, unsigned int step, unsigned int maxEpoch,
                         unsigned int showSpan) override ;

    CAISS_RET_TYPE search(void *handle, void *info, CAISS_SEARCH_TYPE searchType, unsigned int topK) override ;
    CAISS_RET_TYPE getResultSize(void *handle, unsigned int &size) override ;
    CAISS_RET_TYPE getResult(void *handle, char *result, unsigned int size) override ;

    CAISS_RET_TYPE insert(void *handle, CAISS_FLOAT *node, const char *label, CAISS_INSERT_TYPE insertType) override ;
    CAISS_RET_TYPE save(void *handle, const char *modelPath) override ;

private:
    ThreadPool* pool_ptr_;
};


#endif //CAISS_ASYNCMANAGEPROC_H
