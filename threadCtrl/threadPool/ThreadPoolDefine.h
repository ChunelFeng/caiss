//
// Created by Chunel on 2020/6/27.
//

#ifndef CAISS_THREADPOOLDEFINE_H
#define CAISS_THREADPOOLDEFINE_H

#include "../rwLock/RWLock.h"

struct ThreadTaskInfo {
    ThreadTaskInfo(std::function<int()> func, RWLock *rwLock) {
        this->taskFunc = func;
        this->rwLock = rwLock;    // 传入其中的管理类，为了实现在pool中进行信息加锁
    }

    ThreadTaskInfo() {
        this->taskFunc = nullptr;
        this->rwLock = nullptr;
    }

    ThreadTaskInfo& operator= (const ThreadTaskInfo& info) {
        /* 实现赋值构造函数 */
        this->taskFunc = info.taskFunc;
        this->rwLock = info.rwLock;

        return *this;
    }

    ThreadTaskInfo (const ThreadTaskInfo& info) {
        /* 实现拷贝构造函数 */
        this->taskFunc = info.taskFunc;
        this->rwLock = info.rwLock;
    }

    std::function<int()> taskFunc;
    RWLock* rwLock;
};


#endif //CAISS_THREADPOOLDEFINE_H
