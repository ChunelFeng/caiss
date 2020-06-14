//
// Created by Chunel on 2020/5/24.
// 这里直接跟对外暴露的lib接口关联
//

#ifndef CHUNELCAISS_ANNMANAGE_PROC_H
#define CHUNELCAISS_ANNMANAGE_PROC_H

#include <map>

#include "../caissLib/CaissLibDefine.h"
#include "../threadCtrl/ThreadInclude.h"
#include "../algorithmCtrl/AlgorithmInclude.h"

using ManageCtrl = std::map<void*, AlgorithmProc*> ;

class manageProc {
public:
    explicit manageProc(unsigned int maxSize, CAISS_ALGO_TYPE algoType) {
        this->max_size_ = maxSize;
        this->algo_type_ = algoType;
    }

    virtual ~manageProc() = default;

    virtual CAISS_RET_TYPE createHandle(void **handle) = 0;    // 生成处理句柄
    virtual CAISS_RET_TYPE destroyHandle(void *handle) = 0;

    virtual CAISS_RET_TYPE init(void *handle, CAISS_MODE mode, CAISS_DISTANCE_TYPE distanceType, unsigned int dim, const char *modelPath,
                                const CAISS_DIST_FUNC distFunc) = 0;

    virtual CAISS_RET_TYPE train(void *handle, const char *dataPath, const unsigned int maxDataSize, const CAISS_BOOL normalize,
                                 const unsigned int maxIndexSize, const float precision, const unsigned int fastRank,
                                 const unsigned int realRank, const unsigned int step, const unsigned int maxEpoch,
                                 const unsigned int showSpan) = 0;

    virtual CAISS_RET_TYPE search(void *handle, void *info, CAISS_SEARCH_TYPE searchType, unsigned int topK) = 0;
    virtual CAISS_RET_TYPE getResultSize(void *handle, unsigned int &size) = 0;
    virtual CAISS_RET_TYPE getResult(void *handle, char *result, unsigned int size) = 0;

    // label 是数据标签，index表示数据第几个信息
    virtual CAISS_RET_TYPE insert(void *handle, CAISS_FLOAT *node, const char *label, CAISS_INSERT_TYPE insertType) = 0;
    virtual CAISS_RET_TYPE save(void *handle, const char *modelPath) = 0;

protected:
    ManageCtrl free_manage_;    // 没有被用过的句柄管理类
    ManageCtrl used_manage_;    // 被用过的句柄管理类
    RWLock lock_;
    CAISS_ALGO_TYPE algo_type_;
    unsigned int max_size_;
};


#endif //CHUNELCAISS_ANNMANAGE_H
