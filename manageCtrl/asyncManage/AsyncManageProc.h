//
// Created by Chunel on 2020/6/20.
// 开启异步调用流程
//

#ifndef CAISS_ASYNCMANAGEPROC_H
#define CAISS_ASYNCMANAGEPROC_H

#include "../ManageProc.h"

class AsyncManageProc : public ManageProc {
public:
    explicit AsyncManageProc(unsigned int maxSize, CAISS_ALGO_TYPE algoType) : ManageProc(maxSize, algoType) {
        createThreadPoolSingleton(maxSize);
    }

    virtual ~AsyncManageProc() override {
        destroyThreadPoolSingleton();
    }

    CAISS_RET_TYPE train(void *handle, const char *dataPath, unsigned int maxDataSize, CAISS_BOOL normalize,
                         unsigned int maxIndexSize, float precision, unsigned int fastRank,
                         unsigned int realRank, unsigned int step, unsigned int maxEpoch,
                         unsigned int showSpan) override ;

    CAISS_RET_TYPE search(void *handle, void *info, CAISS_SEARCH_TYPE searchType, unsigned int topK, CAISS_SEARCH_CALLBACK searchCBFunc, const void *cbParams) override ;

    CAISS_RET_TYPE save(void *handle, const char *modelPath) override ;

    // label 是数据标签，index表示数据第几个信息
    CAISS_RET_TYPE insert(void *handle, CAISS_FLOAT *node, const char *label, CAISS_INSERT_TYPE insertType) override ;

public:
    static void createThreadPoolSingleton(unsigned int maxSize) {
        if (nullptr == pool_) {
            pool_lock_.writeLock();
            if (nullptr == pool_) {
                pool_ = new ThreadPool(maxSize);
                pool_->start();
            }
            pool_lock_.writeUnlock();
        }

        return;
    }

    static ThreadPool* getThreadPoolSingleton() {
        return pool_;
    }

    static void destroyThreadPoolSingleton() {
        CAISS_DELETE_PTR(pool_)
    }

public:
    static ThreadPool* pool_;
    static RWLock pool_lock_;


};


#endif //CAISS_ASYNCMANAGEPROC_H
