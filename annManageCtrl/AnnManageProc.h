//
// Created by Chunel on 2020/5/24.
// 这里直接跟对外暴露的lib接口关联
//

#ifndef CHUNELANN_ANNMANAGE_PROC_H
#define CHUNELANN_ANNMANAGE_PROC_H

#include <map>

#include "../chunelAnnLib/ChunelAnnLibDefine.h"
#include "../threadCtrl/ThreadInclude.h"
#include "../algorithmCtrl/AlgorithmInclude.h"

using ManageCtrl = std::map<void*, AlgorithmProc*> ;

class AnnManageProc {
public:
    explicit AnnManageProc(unsigned int maxSize, ANN_ALGO_TYPE algoType) {
        this->max_size_ = maxSize;
        this->algo_type_ = algoType;
    }

    virtual ~AnnManageProc() = default;

    virtual ANN_RET_TYPE createHandle(void **handle) = 0;    // 生成处理句柄
    virtual ANN_RET_TYPE destroyHandle(void *handle) = 0;

    virtual ANN_RET_TYPE init(void *handle, ANN_MODE mode, ANN_DISTANCE_TYPE distanceType, unsigned int dim, const char *modelPath,
                              unsigned int exLen) = 0;
    virtual ANN_RET_TYPE search(void *handle, ANN_FLOAT *query, unsigned int topK) = 0;
    virtual ANN_RET_TYPE getResultSize(void *handle, unsigned int &size) = 0;
    virtual ANN_RET_TYPE getResult(void *handle, char *result, unsigned int size) = 0;

    // label 是数据标签，index表示数据第几个信息
    virtual ANN_RET_TYPE insert(void *handle, const ANN_FLOAT *node, const char *label, ANN_INSERT_TYPE insertType) = 0;
    virtual ANN_RET_TYPE save(void *handle, char *modelPath) = 0;

protected:
    ManageCtrl free_manage_;    // 没有被用过的句柄管理类
    ManageCtrl used_manage_;    // 被用过的句柄管理类
    RWLock lock_;
    ANN_ALGO_TYPE algo_type_;
    unsigned int max_size_;
};


#endif //CHUNELANN_ANNMANAGE_H
