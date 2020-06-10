//
// Created by Chunel on 2020/5/24.
//

#ifndef CHUNELCAISS_RWLOCK_H
#define CHUNELCAISS_RWLOCK_H

#include <mutex>

class RWLock {
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
