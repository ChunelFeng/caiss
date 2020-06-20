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
        // @notice 不可以在析构函数里，调用成员函数？
        destroyThreadPoolSingleton();
    }

    virtual CAISS_RET_TYPE train(void *handle, const char *dataPath, unsigned int maxDataSize, CAISS_BOOL normalize,
                         unsigned int maxIndexSize, float precision, unsigned int fastRank,
                         unsigned int realRank, unsigned int step, unsigned int maxEpoch,
                         unsigned int showSpan) override ;

    virtual CAISS_RET_TYPE search(void *handle, void *info, CAISS_SEARCH_TYPE searchType, unsigned int topK) override ;
    virtual CAISS_RET_TYPE getResultSize(void *handle, unsigned int &size) override ;
    virtual CAISS_RET_TYPE getResult(void *handle, char *result, unsigned int size) override ;


public:
    static void createThreadPoolSingleton(unsigned int maxSize) {
        if (nullptr == pool_) {
            pool_lock_.writeLock();
            if (nullptr == pool_) {
                pool_ = new ThreadPool(maxSize);
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
