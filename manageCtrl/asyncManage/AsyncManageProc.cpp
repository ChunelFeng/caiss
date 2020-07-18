//
// Created by Chunel on 2020/6/20.
//

#include "AsyncManageProc.h"

ThreadPool* AsyncManageProc::thread_pool_ = nullptr;
RWLock AsyncManageProc::thread_pool_lock_;

MemoryPool* AsyncManageProc::memory_pool_ = nullptr;
RWLock AsyncManageProc::memory_pool_lock_;


CAISS_RET_TYPE AsyncManageProc::train(void *handle, const char *dataPath, unsigned int maxDataSize, CAISS_BOOL normalize,
                                      unsigned int maxIndexSize, float precision, unsigned int fastRank, unsigned int realRank,
                                      unsigned int step, unsigned int maxEpoch, unsigned int showSpan) {
    CAISS_FUNCTION_BEGIN

    AlgorithmProc *algo = getInstance(handle);
    CAISS_ASSERT_NOT_NULL(algo)

    auto threadPool = getThreadPoolSingleton();
    CAISS_ASSERT_NOT_NULL(threadPool)

    // 绑定训练的流程到线程池中去
    ThreadTaskInfo task(std::bind(&AlgorithmProc::train, algo, dataPath, maxDataSize, normalize, maxIndexSize, precision,
                                  fastRank, realRank, step, maxEpoch, showSpan), this->getRWLock(algo), true);
    threadPool->appendTask(task);
    CAISS_FUNCTION_END
}


CAISS_RET_TYPE AsyncManageProc::search(void *handle,
                                       void *info,
                                       CAISS_SEARCH_TYPE searchType,
                                       unsigned int topK,
                                       const unsigned int filterEditDistance,
                                       const CAISS_SEARCH_CALLBACK searchCBFunc,
                                       const void *cbParams) {
    CAISS_FUNCTION_BEGIN

    AlgorithmProc *algo = getInstance(handle);
    CAISS_ASSERT_NOT_NULL(algo)

    auto threadPool = getThreadPoolSingleton();
    CAISS_ASSERT_NOT_NULL(threadPool)

    void *ptr = nullptr;
    if (searchType == CAISS_SEARCH_WORD || searchType == CAISS_LOOP_WORD) {
        ptr = getMemoryPoolSingleton()->allocate()->data;
        CAISS_ASSERT_NOT_NULL(ptr)
        memset(ptr, 0, BLOCK_SIZE);
        memcpy(ptr, info, strlen((char *)info) + 1);
    } else {
        ptr = info;    // 如果不是单词的话，就是浮点型的vec数据了
    }

    // 查询需要上的锁，属于读锁的性质，其他应该都要上写锁
    ThreadTaskInfo task(std::bind(&AlgorithmProc::search, algo, ptr, searchType, topK, filterEditDistance, searchCBFunc, cbParams), this->getRWLock(algo), false);
    threadPool->appendTask(task);    // 将信息放到线程池中去计算

    CAISS_FUNCTION_END
}


CAISS_RET_TYPE AsyncManageProc::save(void *handle, const char *modelPath) {
    CAISS_FUNCTION_BEGIN

    AlgorithmProc *algo = getInstance(handle);
    CAISS_ASSERT_NOT_NULL(algo)

    auto threadPool = getThreadPoolSingleton();
    CAISS_ASSERT_NOT_NULL(threadPool)

    ThreadTaskInfo task(std::bind(&AlgorithmProc::save, algo, modelPath), this->getRWLock(algo), true);
    threadPool->appendTask(task);

    CAISS_FUNCTION_END
}


// label 是数据标签，index表示数据第几个信息
CAISS_RET_TYPE AsyncManageProc::insert(void *handle, CAISS_FLOAT *node, const char *label, CAISS_INSERT_TYPE insertType) {
    CAISS_FUNCTION_BEGIN

    AlgorithmProc *algo = getInstance(handle);
    CAISS_ASSERT_NOT_NULL(algo)

    auto threadPool = getThreadPoolSingleton();
    CAISS_ASSERT_NOT_NULL(threadPool)

    char *ptr = getMemoryPoolSingleton()->allocate()->data;
    CAISS_ASSERT_NOT_NULL(ptr)
    memset(ptr, 0, BLOCK_SIZE);
    memcpy(ptr, label, strlen(label) + 1);

    ThreadTaskInfo task(std::bind(&AlgorithmProc::insert, algo, node, ptr, insertType), this->getRWLock(algo), true);
    threadPool->appendTask(task);

    CAISS_FUNCTION_END
}


CAISS_RET_TYPE AsyncManageProc::ignore(void *handle, const char *label, bool isIgnore) {
    CAISS_FUNCTION_BEGIN

    AlgorithmProc *algo = getInstance(handle);
    CAISS_ASSERT_NOT_NULL(algo)

    auto threadPool = getThreadPoolSingleton();
    CAISS_ASSERT_NOT_NULL(threadPool)

    char *ptr = getMemoryPoolSingleton()->allocate()->data;
    CAISS_ASSERT_NOT_NULL(ptr)
    memset(ptr, 0, BLOCK_SIZE);
    memcpy(ptr, label, strlen(label) + 1);

    ThreadTaskInfo task(std::bind(&AlgorithmProc::ignore, algo, ptr, isIgnore), this->getRWLock(algo), true);
    threadPool->appendTask(task);

    CAISS_FUNCTION_END
}


RWLock* AsyncManageProc::getRWLock(AlgorithmProc *handle) {
    if (!handle) {
        return nullptr;    // 理论传入的handle不会为空
    }

    RWLock *lock = nullptr;
    if (this->lock_ctrl_.find(handle) != this->lock_ctrl_.end()) {
        lock = this->lock_ctrl_.find(handle)->second;
    }

    return lock;
}
