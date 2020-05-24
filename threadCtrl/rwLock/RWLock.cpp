//
// Created by Chunel on 2020/5/24.
//

#include "RWLock.h"

RWLock::RWLock() {
    this->read_cnt_ = 0;
}

RWLock::~RWLock() {
    this->read_cnt_ = 0;
}

void RWLock::readLock() {
    read_mtx_.lock();
    read_cnt_++;
    if (1 == read_cnt_) {
        write_mtx_.lock();    // 当进入读状态的时候，写锁锁住
    }
    read_mtx_.unlock();
}


void RWLock::writeLock() {
    write_mtx_.lock();
}

void RWLock::readUnlock() {
    read_mtx_.lock();
    read_cnt_--;
    if (0 == read_cnt_) {
        write_mtx_.unlock();    // 没有线程正在读的情况下，将写锁打开
    }

    read_mtx_.unlock();
}

void RWLock::writeUnlock() {
    write_mtx_.unlock();
}
