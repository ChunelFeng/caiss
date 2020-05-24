//
// Created by Chunel on 2020/5/24.
// 这里直接跟对外暴露的lib接口关联
//

#ifndef CHUNELANN_ANNMANAGE_PROC_H
#define CHUNELANN_ANNMANAGE_PROC_H

#include <queue>

#include "../chunelAnnLib/ChunelAnnLibDefine.h"
#include "../threadCtrl/ThreadInclude.h"
#include "../algorithmCtrl/AlgorithmInclude.h"

class AnnManageProc {
public:
    // todo 可以考虑在这里做成让外部多线程，或者是内部支持多线程的版本
    AnnManageProc(unsigned int maxSize) {

    }

    virtual ~AnnManageProc() {

    }

    virtual ANN_RET_TYPE createHandle(void **handle) = 0;    // 生成处理句柄
    virtual ANN_RET_TYPE destroyHandle(void *handle) = 0;

private:
    std::queue<AlgorithmProc*> manages_;
    RWLock lock_;
    unsigned int max_size_;
};


#endif //CHUNELANN_ANNMANAGE_H
