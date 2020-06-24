//
// Created by Chunel on 2020/5/24.
// 这里直接跟对外暴露的lib接口关联
//

#ifndef CHUNELCAISS_ANNMANAGE_PROC_H
#define CHUNELCAISS_ANNMANAGE_PROC_H

#include <map>

#include "./ManageDefine.h"
#include "../threadCtrl/ThreadInclude.h"
#include "../algorithmCtrl/AlgorithmInclude.h"

using ManageCtrl = std::map<void*, AlgorithmProc*> ;

class ManageProc {
public:
    explicit ManageProc(unsigned int maxSize, CAISS_ALGO_TYPE algoType);
    virtual ~ManageProc();
    /* 以下这些函数，是同步/异步走相同的流程 */
    virtual CAISS_RET_TYPE createHandle(void **handle);
    virtual CAISS_RET_TYPE destroyHandle(void* handle);
    virtual CAISS_RET_TYPE init(void *handle, CAISS_MODE mode, CAISS_DISTANCE_TYPE distanceType, unsigned int dim, const char *modelPath,
                                CAISS_DIST_FUNC distFunc);

    virtual CAISS_RET_TYPE save(void *handle, const char *modelPath);
    // label 是数据标签，index表示数据第几个信息
    virtual CAISS_RET_TYPE insert(void *handle, CAISS_FLOAT *node, const char *label, CAISS_INSERT_TYPE insertType);


    /* 以下几个函数，同步和异步需要区分实现 */
    virtual CAISS_RET_TYPE train(void *handle, const char *dataPath, unsigned int maxDataSize, CAISS_BOOL normalize,
                                 unsigned int maxIndexSize, float precision, unsigned int fastRank,
                                 unsigned int realRank, unsigned int step, unsigned int maxEpoch,
                                 unsigned int showSpan) {
        CAISS_FUNCTION_NO_SUPPORT
    }

    virtual CAISS_RET_TYPE search(void *handle, void *info, CAISS_SEARCH_TYPE searchType, unsigned int topK,
                                  CAISS_SEARCH_CALLBACK searchCBFunc, const void *cbParams) {
        CAISS_FUNCTION_NO_SUPPORT
    }

    virtual CAISS_RET_TYPE getResultSize(void *handle, unsigned int &size) {
        CAISS_FUNCTION_NO_SUPPORT
    }

    virtual CAISS_RET_TYPE getResult(void *handle, char *result, unsigned int size) {
        CAISS_FUNCTION_NO_SUPPORT
    }


protected:
    virtual AlgorithmProc* getInstance(void *handle);
    virtual AlgorithmProc* createAlgoProc();


protected:
    ManageCtrl free_manage_;    // 没有被用过的句柄管理类
    ManageCtrl used_manage_;    // 被用过的句柄管理类
    RWLock lock_;
    CAISS_ALGO_TYPE algo_type_;
    unsigned int max_size_;
};


#endif //CHUNELCAISS_ANNMANAGE_H
