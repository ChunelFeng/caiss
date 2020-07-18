//
// Created by Chunel on 2020/7/17.
//

#ifndef CAISS_MEMORYPOOL_H
#define CAISS_MEMORYPOOL_H

#include <mutex>
#include <atomic>
#include <future>

#include "../UtilsProc.h"
#include "MemoryPoolDefine.h"


class MemoryPool : public UtilsProc {
public:
    explicit MemoryPool(unsigned int blockNumPerChunk, unsigned int blockSize);
    ~MemoryPool() override;

    FreeBlock* allocate();
    void deallocate(FreeBlock *block);

protected:
    int innerGC();    // 内部资源回收机制

private:
    unsigned int block_num_per_chunk_;    // 每个chunk中block的个数
    unsigned int block_size_;             // 每个block占用了多少大小

    MemChunk*  head_chunk_;     // 开头的chunk位置
    MemChunk*  cur_chunk_;      // 用于分配的
    FreeBlock* free_block_head_;
    std::mutex mtx_;

    std::atomic<bool> is_finish_{false};
    std::future<int> future_;
};


#endif //CAISS_MEMORYPOOL_H
