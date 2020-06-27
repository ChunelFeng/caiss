//
// Created by Chunel on 2020/5/24.
//

#ifndef CHUNELCAISS_RWLOCK_H
#define CHUNELCAISS_RWLOCK_H

#include <mutex>

enum RWLockType {
    DEFAULT_LOCK_TYPE = 0,    // 用于初始化，无动作发生
    READ_LOCK_TYPE = 1,
    WRITE_LOCK_TYPE = 2,
};

class RWLock {
/* 其实可以考虑，在外面封装一层Ctrl，在作用域内自动销毁锁 */
public:
    RWLock();
    ~RWLock();

    void readLock();    // 加入读锁
    void writeLock();    // 加入写锁
    void readUnlock();
    void writeUnlock();

private:
    int read_cnt_;
    std::mutex read_mtx_;
    std::mutex write_mtx_;
};


#endif //CHUNELCAISS_RWLOCK_H
