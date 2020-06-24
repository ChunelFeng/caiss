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
        unique_lock<mutex> lock(mtx_);
        running_ = false;
        cond_.notify_all();    // 唤起所有线程
    }

    for (auto& t : threads_) {
        if (t.joinable()) {
            t.join();    // 等待所有线程结束
        }
    }
}

void ThreadPool::appendTask(const THREAD_FUNCTION& task) {
    if (running_) {
        unique_lock<mutex> lock(mtx_);
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
        THREAD_FUNCTION curTask = nullptr;
        {
            unique_lock<mutex> lock(mtx_);
            if (running_ && !tasks_.empty()) {
                curTask = tasks_.front();
                tasks_.pop();
            } else if (running_ && tasks_.empty()) {
                cond_.wait(lock);
            }
        }

        if (curTask) {
            work_lock_.writeLock();    // 其中的任务是需要互斥进行的
            curTask();
            work_lock_.writeUnlock();
        }
    }
}

