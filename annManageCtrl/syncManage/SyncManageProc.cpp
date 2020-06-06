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

    this->lock_.writeLock();
    if (used_manage_.find(handle) == used_manage_.end()) {
        this->lock_.writeUnlock();
        return ANN_RET_HANDLE;
    }

    // 给free_manage_重新加入，并且将used_manage_中的对应句柄删除
    // 注明：返回free_manage_中的句柄handle值，跟之前是保持一致的。
    free_manage_.insert(std::make_pair<>((void*)handle, used_manage_.find(handle)->second));
    used_manage_.erase(handle);

    this->lock_.writeUnlock();

    ANN_FUNCTION_END
}


/**
 * 获取内部对应的算法句柄实例
 * @param handle
 * @return
 */
AlgorithmProc* SyncManageProc::getInstance(void *handle) {
    // 通过外部传入的handle信息，转化成内部对应的真实句柄
    AlgorithmProc *proc = nullptr;
    if (this->used_manage_.find(handle) != this->used_manage_.end()) {
        proc = this->used_manage_.find(handle)->second;
    }

    return proc;
}


/**
 * 构造算法句柄信息
 * @return
 */
AlgorithmProc *SyncManageProc::createAlgoProc() {
    AlgorithmProc *proc = nullptr;
    switch (this->algo_type_) {
        case ANN_ALGO_HNSW: proc = new HnswProc(); break;
        case ANN_ALGO_NSG: break;
        default:
            break;
    }

    return proc;
}


/**
 * 初始化所有对应的信息
 * @param maxSize
 * @param algoType
 */
SyncManageProc::SyncManageProc(const unsigned int maxSize, const ANN_ALGO_TYPE algoType)
                              : AnnManageProc(maxSize, algoType) {
    this->max_size_ = 0;
    this->algo_type_ = algoType;
    for(unsigned int i = 0; i < maxSize; i++) {
        AlgorithmProc* proc = createAlgoProc();
        if (nullptr != proc) {
            this->free_manage_.insert(std::make_pair<>((&i + i), proc));
            this->max_size_++;    // 最终包含的算法句柄个数
        }
    }
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


ANN_RET_TYPE SyncManageProc::search(void *handle, ANN_FLOAT *query, unsigned int topK) {
    ANN_FUNCTION_BEGIN

    AlgorithmProc *proc = this->getInstance(handle);
    ANN_ASSERT_NOT_NULL(proc)

    // 查询的时候，使用读锁即可；插入的时候，需要使用写锁
    this->lock_.readLock();
    ret = proc->search(query, ANN_SEARCH_WORD, topK);
    this->lock_.readUnlock();

    ANN_FUNCTION_CHECK_STATUS

    ANN_FUNCTION_END
}


ANN_RET_TYPE SyncManageProc::init(void *handle, ANN_MODE mode, ANN_DISTANCE_TYPE distanceType,
        const unsigned int dim, const char *modelPath, const unsigned int exLen) {
    ANN_FUNCTION_BEGIN

    AlgorithmProc *proc = this->getInstance(handle);
    ANN_ASSERT_NOT_NULL(proc)

    ret = proc->init(mode, distanceType, dim, modelPath, exLen);
    ANN_FUNCTION_CHECK_STATUS

    ANN_FUNCTION_END
}

ANN_RET_TYPE SyncManageProc::getResultSize(void *handle, unsigned int &size) {
    ANN_FUNCTION_BEGIN

    AlgorithmProc *proc = this->getInstance(handle);
    ANN_ASSERT_NOT_NULL(proc)

    ret = proc->getResultSize(size);
    ANN_FUNCTION_CHECK_STATUS

    ANN_FUNCTION_END
}

ANN_RET_TYPE SyncManageProc::getResult(void *handle, char *result, const unsigned int size) {
    ANN_FUNCTION_BEGIN

    AlgorithmProc *proc = this->getInstance(handle);
    ANN_ASSERT_NOT_NULL(proc)

    ret = proc->getResult(result, size);
    ANN_FUNCTION_CHECK_STATUS

    ANN_FUNCTION_END
}

ANN_RET_TYPE SyncManageProc::insert(void *handle, const ANN_FLOAT *node, const char *label, ANN_INSERT_TYPE insertType) {
    ANN_FUNCTION_BEGIN

    AlgorithmProc *proc = this->getInstance(handle);
    ANN_ASSERT_NOT_NULL(proc)

    this->lock_.writeLock();
    ret = proc->insert(node, label, insertType);
    this->lock_.writeUnlock();
    ANN_FUNCTION_CHECK_STATUS

    ANN_FUNCTION_END
}


ANN_RET_TYPE SyncManageProc::save(void *handle, char *modelPath) {
    ANN_FUNCTION_BEGIN

    AlgorithmProc *proc = this->getInstance(handle);
    ANN_ASSERT_NOT_NULL(proc)

    this->lock_.writeLock();
    ret = proc->save(modelPath);
    this->lock_.writeUnlock();
    ANN_FUNCTION_CHECK_STATUS

    ANN_FUNCTION_END
}




