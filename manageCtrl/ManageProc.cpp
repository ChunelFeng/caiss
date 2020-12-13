//
// Created by Chunel on 2020/6/20.
//

#include "ManageProc.h"

ManageProc::ManageProc(unsigned int maxSize, CAISS_ALGO_TYPE algoType) {
    this->max_size_ = maxSize;
    this->algo_type_ = algoType;

    for(unsigned int i = 0; i < maxSize; i++) {
        AlgorithmProc* proc = createAlgoProc();
        if (nullptr != proc) {
            this->free_manage_.insert(std::make_pair<>((&i + i), proc));
        }
    }
}


ManageProc::~ManageProc() {
    for (auto i : free_manage_) {
        CAISS_DELETE_PTR(i.second);
    }

    for (auto j : used_manage_) {
        CAISS_DELETE_PTR(j.second);
    }
}


CAISS_STATUS ManageProc::createHandle(void **handle) {
    CAISS_FUNCTION_BEGIN

    this->lock_.writeLock();    // 如果内部还有句柄信息的话，开始分配句柄操作
    if (this->free_manage_.empty()) {
        this->lock_.writeUnlock();
        return CAISS_RET_HANDLE;    // 如果是空，表示返回失败
    }

    void* key = free_manage_.begin()->first;
    AlgorithmProc* proc = free_manage_.begin()->second;
    free_manage_.erase(key);
    used_manage_.insert(std::make_pair<>(key, proc));

    *handle = key;
    this->lock_.writeUnlock();
    CAISS_FUNCTION_END
}


CAISS_STATUS ManageProc::destroyHandle(void* handle) {
    CAISS_FUNCTION_BEGIN
    CAISS_ASSERT_NOT_NULL(handle);

    this->lock_.writeLock();
    if (used_manage_.find(handle) == used_manage_.end()) {
        this->lock_.writeUnlock();
        return CAISS_RET_HANDLE;
    }

    // 给free_manage_重新加入，并且将used_manage_中的对应句柄删除
    // 注明：返回free_manage_中的句柄handle值，跟之前是保持一致的。
    free_manage_.insert(std::make_pair<>((void*)handle, used_manage_.find(handle)->second));
    used_manage_.erase(handle);

    this->lock_.writeUnlock();
    CAISS_FUNCTION_END
}


CAISS_STATUS ManageProc::init(void *handle, CAISS_MODE mode, CAISS_DISTANCE_TYPE distanceType, unsigned int dim, const char *modelPath,
                              CAISS_DIST_FUNC distFunc) {
    CAISS_FUNCTION_BEGIN

    AlgorithmProc *proc = this->getInstance(handle);
    CAISS_ASSERT_NOT_NULL(proc)

    this->lock_.writeLock();
    ret = proc->init(mode, distanceType, dim, modelPath, distFunc);
    this->lock_.writeUnlock();
    CAISS_FUNCTION_CHECK_STATUS

    model_path_ = std::string(modelPath);

    CAISS_FUNCTION_END
}


/**
 * 传入锁的类型，在ThreadPool中实现加锁和解锁
 * @param action
 */
void ManageProc::doLock(RWLockType action) {
    switch (action) {
        case DEFAULT_LOCK_TYPE: break;
        case READ_LOCK_TYPE: this->lock_.readLock(); break;
        case WRITE_LOCK_TYPE: this->lock_.writeLock(); break;
        default:
            break;
    }
}


/**
 *
 * @param action
 */
void ManageProc::doUnlock(RWLockType action) {
    switch (action) {
        case DEFAULT_LOCK_TYPE: break;
        case READ_LOCK_TYPE: this->lock_.readUnlock(); break;
        case WRITE_LOCK_TYPE: this->lock_.writeUnlock(); break;
        default:
            break;
    }
}


/* 以下是内部的函数 */
AlgorithmProc* ManageProc::getInstance(void *handle) {
    // 通过外部传入的handle信息，转化成内部对应的真实句柄
    AlgorithmProc *proc = nullptr;
    if (this->used_manage_.find(handle) != this->used_manage_.end()) {
        proc = this->used_manage_.find(handle)->second;
    }

    return proc;
}


RWLock* ManageProc::getRWLock(AlgorithmProc *proc) {
    return nullptr;    // 仅在异步调用的时候，会被涉及到。这里的实现，理论不会出现这种情况
}


AlgorithmProc* ManageProc::createAlgoProc() {
    AlgorithmProc *proc = nullptr;
    switch (this->algo_type_) {
        case CAISS_ALGO_HNSW: proc = new HnswProc(); break;
        case CAISS_ALGO_MRPT: proc = new MrptProc(); break;
        default:
            break;
    }

    return proc;
}



