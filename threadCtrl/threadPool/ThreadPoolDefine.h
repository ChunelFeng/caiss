//
// Created by Chunel on 2020/6/27.
//

#ifndef CAISS_THREADPOOLDEFINE_H
#define CAISS_THREADPOOLDEFINE_H

#include "../rwLock/RWLock.h"
#include "../../utilsCtrl/UtilsInclude.h"

struct ThreadTaskInfo {
    ThreadTaskInfo(std::function<int()> func, RWLock *rwLock, bool isUniq, MemoryPool *memPool, FreeBlock *block) {
        this->taskFunc = func;
        this->rwLock = rwLock;    // 传入其中的管理类，为了实现在pool中进行信息加锁
        this->isUniq = isUniq;
        this->memPool = memPool;
        this->block = block;
    }

    ThreadTaskInfo() {
        this->taskFunc = nullptr;
        this->rwLock = nullptr;
        this->isUniq = false;
        this->memPool = nullptr;
        this->block = nullptr;
    }

    ThreadTaskInfo& operator= (const ThreadTaskInfo& info) {
        /* 实现赋值构造函数 */
        this->taskFunc = info.taskFunc;
        this->rwLock = info.rwLock;
        this->isUniq = info.isUniq;
        this->memPool = info.memPool;
        this->block = info.block;
        return *this;
    }

    ThreadTaskInfo(const ThreadTaskInfo& info) {
        /* 实现拷贝构造函数 */
        this->taskFunc = info.taskFunc;
        this->rwLock = info.rwLock;
        this->isUniq = info.isUniq;
        this->memPool = info.memPool;
        this->block = info.block;
    }

    std::function<int()> taskFunc;
    RWLock* rwLock;
    bool isUniq;    // 是否是独占的执行函数
    MemoryPool* memPool;
    FreeBlock* block;
};


#endif //CAISS_THREADPOOLDEFINE_H
