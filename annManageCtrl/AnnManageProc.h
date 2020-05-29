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
    // todo 可以考虑在这里做成让外部多线程，或者是内部支持多线程的版本
    explicit AnnManageProc(unsigned int maxSize) {
        this->max_size_ = maxSize;
    }

    virtual ~AnnManageProc() = default;

    virtual ANN_RET_TYPE createHandle(void **handle) = 0;    // 生成处理句柄
    virtual ANN_RET_TYPE destroyHandle(void *handle) = 0;

    ManageCtrl free_manage_;    // 没有被用过的句柄管理类
    ManageCtrl used_manage_;    // 被用过的句柄管理类

protected:
    RWLock lock_;
    unsigned int max_size_;
};


#endif //CHUNELANN_ANNMANAGE_H
