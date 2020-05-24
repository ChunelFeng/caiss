//
// Created by Chunel on 2020/5/24.
//

#ifndef CHUNELANN_RWLOCK_H
#define CHUNELANN_RWLOCK_H



#include <mutex>

class RWLock {
public:
    RWLock();
    ~RWLock();

    void readLock();
    void writeLock();
    void readUnlock();
    void writeUnlock();

protected:


private:
    int read_cnt_;
    std::mutex read_mtx_;
    std::mutex write_mtx_;
};


#endif //CHUNELANN_RWLOCK_H
