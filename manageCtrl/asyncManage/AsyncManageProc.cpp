//
// Created by Chunel on 2020/6/20.
//

#include "AsyncManageProc.h"

ThreadPool* AsyncManageProc::pool_ = nullptr;
RWLock AsyncManageProc::pool_lock_;


CAISS_RET_TYPE AsyncManageProc::train(void *handle, const char *dataPath, unsigned int maxDataSize, CAISS_BOOL normalize,
                       unsigned int maxIndexSize, float precision, unsigned int fastRank, unsigned int realRank,
                       unsigned int step, unsigned int maxEpoch, unsigned int showSpan) {
    CAISS_FUNCTION_BEGIN

    AlgorithmProc *algo = getInstance(handle);
    CAISS_ASSERT_NOT_NULL(algo)

    auto pool = getThreadPoolSingleton();
    CAISS_ASSERT_NOT_NULL(pool)

    // 绑定训练的流程到线程池中去
    pool->appendTask(std::bind(&AlgorithmProc::train, algo, dataPath, maxDataSize, normalize, maxIndexSize, precision,
            fastRank, realRank, step, maxEpoch, showSpan));

    CAISS_FUNCTION_END
}


CAISS_RET_TYPE AsyncManageProc::search(void *handle, void *info, CAISS_SEARCH_TYPE searchType, unsigned int topK) {
    CAISS_FUNCTION_BEGIN

    AlgorithmProc *algo = getInstance(handle);
    CAISS_ASSERT_NOT_NULL(algo)

    auto pool = getThreadPoolSingleton();
    CAISS_ASSERT_NOT_NULL(pool)
    pool->appendTask(std::bind(&AlgorithmProc::search, algo, info, searchType, topK));    // 将信息放到线程池中去计算

    CAISS_FUNCTION_END
}


CAISS_RET_TYPE AsyncManageProc::getResultSize(void *handle, unsigned int &size) {
    CAISS_FUNCTION_END
}


CAISS_RET_TYPE AsyncManageProc::getResult(void *handle, char *result, unsigned int size) {
    CAISS_FUNCTION_END
}
