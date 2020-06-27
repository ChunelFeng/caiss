//
// Created by Chunel on 2020/6/27.
//

#ifndef CAISS_THREADPOOLDEFINE_H
#define CAISS_THREADPOOLDEFINE_H

#include "../rwLock/RWLock.h"

struct ThreadTaskInfo {
    ThreadTaskInfo(std::function<int()> func, void *manage, RWLockType lockType) {
        this->taskFunc = func;
        this->taskManage = manage;    // 传入其中的管理类，为了实现在pool中进行信息加锁
        this->lockType = lockType;
    }

    ThreadTaskInfo() {
        this->taskFunc = nullptr;
        this->taskManage = nullptr;
        this->lockType = WRITE_LOCK_TYPE;    // 用于初始化的状态
    }

    ThreadTaskInfo& operator= (const ThreadTaskInfo& info) {
        /* 实现赋值构造函数 */
        this->taskFunc = info.taskFunc;
        this->taskManage = info.taskManage;
        this->lockType = info.lockType;

        return *this;
    }

    ThreadTaskInfo (const ThreadTaskInfo& info) {
        /* 实现拷贝构造函数 */
        this->taskFunc = info.taskFunc;
        this->taskManage = info.taskManage;
        this->lockType = info.lockType;
    }

    std::function<int()> taskFunc;
    void* taskManage;
    RWLockType lockType;
};


#endif //CAISS_THREADPOOLDEFINE_H
