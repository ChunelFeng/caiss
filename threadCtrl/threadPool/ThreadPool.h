//
// Created by Chunel on 2020/6/20.
// 写一个线程池，支持上层异步调用逻辑
//

#ifndef CAISS_THREADPOOL_H
#define CAISS_THREADPOOL_H

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <vector>
#include <queue>
#include "../rwLock/RWLock.h"

using namespace std;


class ThreadPool {
    using THREAD_FUNCTION = std::function<int()>;

public:
    explicit ThreadPool(unsigned int num) :
    thread_num_(num),
    running_(false) {
    }

    ~ThreadPool() {
        if (running_) {
            this->stop();
        }
    }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator= (const ThreadPool& pool) = delete;

    void start();
    void stop();
    void appendTask(const THREAD_FUNCTION& task);

protected:
    void work();

private:
    atomic_bool running_;
    mutex mtx_;
    condition_variable cond_;
    unsigned int thread_num_;
    vector<thread> threads_;    // 线程数组
    queue<THREAD_FUNCTION> tasks_;    // 任务队列
    RWLock work_lock_;
};


#endif //CAISS_THREADPOOL_H
