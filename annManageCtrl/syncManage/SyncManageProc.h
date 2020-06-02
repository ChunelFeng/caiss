//
// Created by Chunel on 2020/5/24.
//

#ifndef CHUNELANN_SYNCMANAGEPROC_H
#define CHUNELANN_SYNCMANAGEPROC_H

#include "../AnnManageProc.h"

class SyncManageProc : public AnnManageProc  {
public:
    explicit SyncManageProc(unsigned int maxSize, ANN_ALGO_TYPE algoType);
    ~SyncManageProc() override ;

    ANN_RET_TYPE createHandle(void **handle) override ;    // 生成处理句柄
    ANN_RET_TYPE destroyHandle(void *handle) override ;

    ANN_RET_TYPE init(void *handle, ANN_MODE mode, ANN_DISTANCE_TYPE distanceType,
                      unsigned int dim, const char *modelPath, unsigned int exLen) override ;
    ANN_RET_TYPE search(void *handle, ANN_FLOAT *query, unsigned int topK) override ;
    ANN_RET_TYPE getResultSize(void *handle, unsigned int &size) override ;
    ANN_RET_TYPE getResult(void *handle, char *result, unsigned int size) override ;

    ANN_RET_TYPE insert(void *handle, const ANN_FLOAT *node, const char *label, ANN_INSERT_TYPE insertType) override ;
    ANN_RET_TYPE save(void *handle, char *modelPath) override ;

protected:
    AlgorithmProc* getInstance(void *handle);    // 同步方式下，通过传入的handle，返回具体的算法处理类
    AlgorithmProc* createAlgoProc();
};


#endif //CHUNELANN_SYNCMANAGEPROC_H
