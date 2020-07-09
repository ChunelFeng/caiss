//
// Created by Chunel on 2020/6/20.
//

#include <iostream>
#include "ThreadPool.h"

using namespace std;

void ThreadPool::start() {
    running_ = true;    // 一定是设定true，再开启线程
    for (unsigned int i = 0; i < thread_num_; i++) {
        threads_.emplace_back(thread(&ThreadPool::work, this));    // 每个线程都运行work函数
    }
}

void ThreadPool::stop() {
    {
        unique_lock<mutex> lock(pool_mtx_);
        running_ = false;
        cond_.notify_all();    // 唤起所有线程
    }

    for (auto& t : threads_) {
        if (t.joinable()) {
            t.join();    // 等待所有线程结束
        }
    }
}

void ThreadPool::appendTask(const ThreadTaskInfo& task) {
    if (running_) {
        unique_lock<mutex> lock(pool_mtx_);
        if (!task.taskFunc || !task.rwLock) {
            return;    // 如果传入的任务中，信息为空，则不进入消息队列
        }

        tasks_.push(task);
        cond_.notify_one();
    }
}

/**
 * 每个线程都一直在运行这个函数。
 * 当有task进来的时候，就获取第一个task，并且执行。
 * 如果没有需要执行的tark了，则一直处于wait的状态，直到有调用appendTask，从而cond被唤醒
 */
void ThreadPool::work() {
    while (running_) {
        ThreadTaskInfo curTask;
        {
            unique_lock<mutex> lock(pool_mtx_);
            if (running_ && !tasks_.empty()) {
                curTask = tasks_.front();
                tasks_.pop();
            } else if (running_ && tasks_.empty()) {
                cond_.wait(lock);
            }
        }

        curTask.isUniq ? this->func_lock_.writeLock() : this->func_lock_.readLock();    // work函数是在不同的thread中运行的，不会出事的
        auto *lck = (RWLock *)curTask.rwLock;
        lck->writeLock();    // 这里必须用write-lock，是因为需要确保，同一个算法句柄，不会被两个线程进入两次
        curTask.taskFunc();
        lck->writeUnlock();
        curTask.isUniq ? this->func_lock_.writeUnlock() : this->func_lock_.readUnlock();
    }
}

