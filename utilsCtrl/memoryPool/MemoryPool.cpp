//
// Created by Chunel on 2020/7/17.
//

#include <thread>
#include "MemoryPool.h"

int MemoryPool::innerGC() {
    // 暂时将SLEEP_TIME_SECOND设置为INT_MAX表示不会进入下面的自动清除内存的逻辑
    while (!is_finish_) {
        int i = 0;
        while ((++i < SLEEP_TIME_SECOND) && !is_finish_) {
            std::this_thread::sleep_for(std::chrono::seconds(1));    // 分批次休眠SLEEP_TIME_SPAN秒
        }

        if (is_finish_) {
            break;
        }

        std::lock_guard<std::mutex> lock(mtx_);
        int count = 0;
        MemChunk *p = head_chunk_;
        while (p) {
            count++;
            p = p->next;
        }

        if (count > MAX_CHUNK_SIZE) {
            int delNum = count / 10;    // 清除十分之一的数据
            while (delNum--) {
                MemChunk *cur = head_chunk_;
                head_chunk_ = head_chunk_->next;
                delete cur;
            }
        }
    }

    return 0;
}

MemoryPool::MemoryPool(unsigned int blockNumPerChunk, unsigned int blockSize) {
    std::lock_guard<std::mutex> lock(mtx_);
    head_chunk_ = nullptr;
    cur_chunk_ = nullptr;
    free_block_head_ = nullptr;
    block_size_ = blockSize;
    block_num_per_chunk_ = blockNumPerChunk;
    is_finish_ = false;
    //future_ = std::async(std::launch::async, &MemoryPool::innerGC, this);    // 暂不开启自动回收机制
}

MemoryPool::~MemoryPool() {
    is_finish_ = true;    // 保证gc线程终止
    //future_.get();

    std::lock_guard<std::mutex> lock(mtx_);
    MemChunk *p;
    while (head_chunk_) {
        p = head_chunk_->next;
        delete head_chunk_;
        head_chunk_ = p;
    }
}

FreeBlock *MemoryPool::allocate() {
    std::lock_guard<std::mutex> lock(mtx_);
    if (!free_block_head_) {
        // 如果空闲节点的信息为空了
        auto *curChunk = new MemChunk(this->block_num_per_chunk_, this->block_size_);
        free_block_head_ = curChunk->blocks[0];    // 获得新的Chunk的信息，作为头结点
        for (unsigned int i = 1; i < this->block_num_per_chunk_; i++) {
            curChunk->blocks[i-1]->next = curChunk->blocks[i];
        }

        if (!head_chunk_) {
            head_chunk_ = curChunk;    // 如果mem_chunk_head_是null的话，对其赋值
            cur_chunk_ = curChunk;
        } else {
            cur_chunk_->next = curChunk;    // 如果有的话，cur值后移一位
            cur_chunk_ = cur_chunk_->next;
        }
    }

    FreeBlock *obj = free_block_head_;    // 将当前节点的信息赋值出来
    free_block_head_ = free_block_head_->next;

    return obj;
}

void MemoryPool::deallocate(FreeBlock *block) {
    if (nullptr == block) {
        return;    // 如果内存没被自动回收的话，就后移。但是内存有可能被自动回收，所以在这里加入判断
    }

    std::lock_guard<std::mutex> lock(mtx_);
    memset(block->data, 0, this->block_size_);
    block->next = free_block_head_;    // just insert into free block list, but not release...
    free_block_head_ = block;
}
