//
// Created by Chunel on 2020/5/24.
//


#include "SyncManageProc.h"

CAISS_RET_TYPE SyncManageProc::createHandle(void **handle) {
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

CAISS_RET_TYPE SyncManageProc::destroyHandle(void* handle) {
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
        case CAISS_ALGO_HNSW: proc = new HnswProc(); break;
        case CAISS_ALGO_NSG: break;
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
SyncManageProc::SyncManageProc(const unsigned int maxSize, CAISS_ALGO_TYPE algoType)
                              : manageProc(maxSize, algoType) {
    for(unsigned int i = 0; i < maxSize; i++) {
        AlgorithmProc* proc = createAlgoProc();
        if (nullptr != proc) {
            this->free_manage_.insert(std::make_pair<>((&i + i), proc));
        }
    }
}


SyncManageProc::~SyncManageProc() {
    for (auto i : free_manage_) {
        CAISS_DELETE_PTR(i.second);
    }

    for (auto j : used_manage_) {
        CAISS_DELETE_PTR(j.second);
    }

}


CAISS_RET_TYPE SyncManageProc::search(void *handle, void *info, CAISS_SEARCH_TYPE searchType, unsigned int topK) {
    CAISS_FUNCTION_BEGIN

    AlgorithmProc *proc = this->getInstance(handle);
    CAISS_ASSERT_NOT_NULL(proc)

    // 查询的时候，使用读锁即可；插入的时候，需要使用写锁
    this->lock_.readLock();
    ret = proc->search(info, searchType, topK);
    this->lock_.readUnlock();

    CAISS_FUNCTION_CHECK_STATUS

    CAISS_FUNCTION_END
}


CAISS_RET_TYPE SyncManageProc::init(void *handle, CAISS_MODE mode, CAISS_DISTANCE_TYPE distanceType,
                                    unsigned int dim, const char *modelPath, const CAISS_DIST_FUNC distFunc) {
    CAISS_FUNCTION_BEGIN

    AlgorithmProc *proc = this->getInstance(handle);
    CAISS_ASSERT_NOT_NULL(proc)

    ret = proc->init(mode, distanceType, dim, modelPath, distFunc);
    CAISS_FUNCTION_CHECK_STATUS

    CAISS_FUNCTION_END
}

CAISS_RET_TYPE SyncManageProc::getResultSize(void *handle, unsigned int &size) {
    CAISS_FUNCTION_BEGIN

    AlgorithmProc *proc = this->getInstance(handle);
    CAISS_ASSERT_NOT_NULL(proc)

    ret = proc->getResultSize(size);
    CAISS_FUNCTION_CHECK_STATUS

    CAISS_FUNCTION_END
}

CAISS_RET_TYPE SyncManageProc::getResult(void *handle, char *result, const unsigned int size) {
    CAISS_FUNCTION_BEGIN

    AlgorithmProc *proc = this->getInstance(handle);
    CAISS_ASSERT_NOT_NULL(proc)

    ret = proc->getResult(result, size);
    CAISS_FUNCTION_CHECK_STATUS

    CAISS_FUNCTION_END
}

CAISS_RET_TYPE SyncManageProc::insert(void *handle, CAISS_FLOAT *node, const char *label, CAISS_INSERT_TYPE insertType) {
    CAISS_FUNCTION_BEGIN

    AlgorithmProc *proc = this->getInstance(handle);
    CAISS_ASSERT_NOT_NULL(proc)

    this->lock_.writeLock();
    ret = proc->insert(node, label, insertType);
    this->lock_.writeUnlock();
    CAISS_FUNCTION_CHECK_STATUS

    CAISS_FUNCTION_END
}


CAISS_RET_TYPE SyncManageProc::save(void *handle, const char *modelPath) {
    CAISS_FUNCTION_BEGIN

    AlgorithmProc *proc = this->getInstance(handle);
    CAISS_ASSERT_NOT_NULL(proc)

    this->lock_.writeLock();
    ret = proc->save(modelPath);
    this->lock_.writeUnlock();
    CAISS_FUNCTION_CHECK_STATUS

    CAISS_FUNCTION_END
}

CAISS_RET_TYPE SyncManageProc::train(void *handle, const char *dataPath, const unsigned int maxDataSize, CAISS_BOOL normalize,
                      const unsigned int maxIndexSize, const float precision, const unsigned int fastRank,
                      const unsigned int realRank, const unsigned int step, const unsigned int maxEpoch,
                      const unsigned int showSpan) {
    CAISS_FUNCTION_BEGIN

    AlgorithmProc *proc = this->getInstance(handle);
    CAISS_ASSERT_NOT_NULL(proc)

    this->lock_.writeLock();
    ret = proc->train(dataPath, maxDataSize, normalize, maxIndexSize, precision, fastRank, realRank, step, maxEpoch, showSpan);
    this->lock_.writeUnlock();

    CAISS_FUNCTION_CHECK_STATUS

    CAISS_FUNCTION_END
}




