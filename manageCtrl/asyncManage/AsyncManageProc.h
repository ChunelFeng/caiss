//
// Created by Chunel on 2020/6/20.
// 开启异步调用流程
//

#ifndef CAISS_ASYNCMANAGEPROC_H
#define CAISS_ASYNCMANAGEPROC_H



#include "../ManageProc.h"
using AsyncLockCtrl = std::map<AlgorithmProc *, RWLock *>;    // 异步操作的时候，会被用到

/*
 * 超参，BLOCK_NUM_PER_CHUNK表示一个chunk中，默认有多少个block。其中，每个block的大小，为BLOCK_SIZE值
 * */
const static unsigned int BLOCK_NUM_PER_CHUNK = 8;
const static unsigned int BLOCK_SIZE = 256;

class AsyncManageProc : public ManageProc {
public:
    explicit AsyncManageProc(unsigned int maxSize, CAISS_ALGO_TYPE algoType) : ManageProc(maxSize, algoType) {
        createThreadPoolSingleton(maxSize);    // 分配线程池信息
        // 在这里，需要给每个句柄分配锁资源
        for (auto& cur : free_manage_) {
            auto* lck = new RWLock();
            this->lock_ctrl_.insert(std::make_pair<>(cur.second, lck));
        }

        createMemoryPoolSingleton();    // 分配内存池信息
    }

    ~AsyncManageProc() override {
        for (auto& cur : lock_ctrl_) {
            CAISS_DELETE_PTR(cur.second);
        }

        destroyThreadPoolSingleton();
        destroyMemoryPoolSingleton();
    }

    CAISS_RET_TYPE train(void *handle, const char *dataPath, unsigned int maxDataSize, CAISS_BOOL normalize,
                         unsigned int maxIndexSize, float precision, unsigned int fastRank,
                         unsigned int realRank, unsigned int step, unsigned int maxEpoch,
                         unsigned int showSpan) override ;

    CAISS_RET_TYPE search(void *handle, void *info, CAISS_SEARCH_TYPE searchType,
                          unsigned int topK, unsigned int filterEditDistance,
                          CAISS_SEARCH_CALLBACK searchCBFunc, const void *cbParams) override ;

    CAISS_RET_TYPE save(void *handle, const char *modelPath) override ;

    // label 是数据标签，index表示数据第几个信息
    CAISS_RET_TYPE insert(void *handle, CAISS_FLOAT *node, const char *label, CAISS_INSERT_TYPE insertType) override ;

    CAISS_RET_TYPE ignore(void *handle, const char *label, CAISS_BOOL isIgnore) override ;

    RWLock* getRWLock(AlgorithmProc * handle) override ;

public:
    static void createThreadPoolSingleton(unsigned int maxSize) {
        if (nullptr == thread_pool_) {
            thread_pool_lock_.writeLock();
            if (nullptr == thread_pool_) {
                thread_pool_ = new ThreadPool(maxSize);
                thread_pool_->start();
            }
            thread_pool_lock_.writeUnlock();
        }
    }

    static ThreadPool* getThreadPoolSingleton() {
        return thread_pool_;
    }

    static void destroyThreadPoolSingleton() {
        CAISS_DELETE_PTR(thread_pool_)
    }

    static void createMemoryPoolSingleton() {
        if (nullptr == memory_pool_) {
            memory_pool_lock_.writeLock();
            if (nullptr == memory_pool_) {
                memory_pool_ = new MemoryPool(BLOCK_NUM_PER_CHUNK, BLOCK_SIZE);
            }
            memory_pool_lock_.writeUnlock();
        }
    }

    static MemoryPool* getMemoryPoolSingleton() {
        return memory_pool_;
    }

    static void destroyMemoryPoolSingleton() {
        CAISS_DELETE_PTR(memory_pool_)
    }

public:
    static ThreadPool* thread_pool_;
    static RWLock thread_pool_lock_;

    static MemoryPool* memory_pool_;
    static RWLock memory_pool_lock_;

private:
    AsyncLockCtrl lock_ctrl_;
};


#endif //CAISS_ASYNCMANAGEPROC_H
