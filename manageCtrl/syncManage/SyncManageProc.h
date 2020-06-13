//
// Created by Chunel on 2020/5/24.
//

#ifndef CHUNELCAISS_SYNCMANAGEPROC_H
#define CHUNELCAISS_SYNCMANAGEPROC_H

#include "../manageProc.h"

class SyncManageProc : public manageProc  {
public:
    explicit SyncManageProc(const unsigned int maxSize, CAISS_ALGO_TYPE algoType);
    ~SyncManageProc() override ;

    CAISS_RET_TYPE createHandle(void **handle) override ;    // 生成处理句柄
    CAISS_RET_TYPE destroyHandle(void *handle) override ;

    CAISS_RET_TYPE init(void *handle, CAISS_MODE mode, CAISS_DISTANCE_TYPE distanceType,
                        unsigned int dim, const char *modelPath, const CAISS_DIST_FUNC distFunc) override ;
    CAISS_RET_TYPE search(void *handle, void *info, CAISS_SEARCH_TYPE searchType, unsigned int topK) override ;
    CAISS_RET_TYPE getResultSize(void *handle, unsigned int &size) override ;
    CAISS_RET_TYPE getResult(void *handle, char *result, unsigned int size) override ;

    CAISS_RET_TYPE insert(void *handle, CAISS_FLOAT *node, const char *label, CAISS_INSERT_TYPE insertType) override ;
    CAISS_RET_TYPE save(void *handle, char *modelPath) override ;

protected:
    AlgorithmProc* getInstance(void *handle);    // 同步方式下，通过传入的handle，返回具体的算法处理类
    AlgorithmProc* createAlgoProc();
};


#endif //CHUNELCAISS_SYNCMANAGEPROC_H
