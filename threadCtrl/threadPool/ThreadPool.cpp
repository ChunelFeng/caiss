//
// Created by Chunel on 2020/6/20.
//

#include <iostream>
#include "ThreadPool.h"
#include "../../manageCtrl/ManageInclude.h"

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

        if (curTask.taskFunc && curTask.taskManage && DEFAULT_LOCK_TYPE != curTask.lockType) {
            auto* manage = (AsyncManageProc*)curTask.taskManage;
            manage->doLock(curTask.lockType);    // 根据传入的类型，进行加解锁操作
            curTask.taskFunc();
            manage->doUnlock(curTask.lockType);
        }
    }
}

