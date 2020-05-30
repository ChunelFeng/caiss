//
// Created by Chunel on 2020/5/24.
//


#include "SyncManageProc.h"

ANN_RET_TYPE SyncManageProc::createHandle(void** handle) {
    ANN_FUNCTION_BEGIN

    if (this->free_manage_.empty()) {
        return ANN_RET_HANDLE;    // 如果是空，表示返回失败
    }

    this->lock_.writeLock();    // 如果内部还有句柄信息的话，开始分配句柄操作

    void* key = free_manage_.begin()->first;
    AlgorithmProc* proc = free_manage_.begin()->second;
    free_manage_.erase(key);
    used_manage_.insert(std::make_pair<>(key, proc));

    *handle = key;
    this->lock_.writeUnlock();
    ANN_FUNCTION_END
}

ANN_RET_TYPE SyncManageProc::destroyHandle(void* handle) {
    ANN_FUNCTION_BEGIN
    ANN_ASSERT_NOT_NULL(handle);

    if (used_manage_.find(handle) == used_manage_.end()) {
        return ANN_RET_HANDLE;
    }

    this->lock_.writeLock();

    // 给free_manage_重新加入，并且将used_manage_中的对应句柄删除
    int i = 0;
    free_manage_.insert(std::make_pair<>((void*)&i, used_manage_.find(handle)->second));
    used_manage_.erase(handle);

    this->lock_.writeUnlock();

    ANN_FUNCTION_END
}

/**
 * 初始化所有对应的信息
 * @param maxSize
 */
SyncManageProc::SyncManageProc(unsigned int maxSize) : AnnManageProc(maxSize) {
    used_manage_.clear();
    for(unsigned int i = 0; i < maxSize; i++) {
        AlgorithmProc* proc = new HnswProc();    // 对应的算法的句柄，需要配置
        free_manage_.insert(std::make_pair<>((&i + i), proc));
    }

    this->max_size_ = maxSize;
}

SyncManageProc::~SyncManageProc() {
    for (auto i : free_manage_) {
        ANN_DELETE_PTR(i.second);
    }

    for (auto j : used_manage_) {
        ANN_DELETE_PTR(j.second);
    }

    this->max_size_ = 0;
}

AlgorithmProc* SyncManageProc::getInstance(void *handle) {
    // 通过外部传入的handle信息，转化成内部对应的真实句柄
    AlgorithmProc *proc = nullptr;
    if (this->used_manage_.find(handle) != this->used_manage_.end()) {
        proc = this->used_manage_.find(handle)->second;
    }

    return proc;
}
