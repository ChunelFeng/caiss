//
// Created by Chunel on 2020/5/24.
//


#include "SyncManageProc.h"

ANN_RET_TYPE SyncManageProc::createHandle(void** handle) {
    ANN_FUNCTION_BEGIN

    this->lock_.writeLock();    // 如果内部还有句柄信息的话，开始分配句柄操作

    if (this->free_manage_.empty()) {
        this->lock_.writeUnlock();
        return ANN_RET_HANDLE;    // 如果是空，表示返回失败
    }

    void* key = free_manage_.begin()->first;
    AlgorithmProc* value = free_manage_.begin()->second;
    free_manage_.erase(key);
    used_manage_.insert(std::make_pair<>(key, value));

    *handle = key;
    this->lock_.writeUnlock();
    ANN_FUNCTION_END
}

ANN_RET_TYPE SyncManageProc::destroyHandle(void* handle) {
    //ANN_ASSERT_NOT_NULL(handle);
    ANN_FUNCTION_BEGIN

    this->lock_.writeLock();

    if (used_manage_.find(handle) == used_manage_.end()) {
        this->lock_.writeUnlock();    // 如果这个句柄没有被用到过
        return ANN_RET_HANDLE;
    }

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
    for(unsigned int i = 0; i < maxSize; i++) {
        AlgorithmProc* pc = new HnswProc();    // 对应的算法的句柄，需要配置
        free_manage_.insert(std::make_pair<>((&i + i), pc));
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
}
