//
// Created by Chunel on 2020/7/17.
//

#ifndef CAISS_MEMORYPOOLDEFINE_H
#define CAISS_MEMORYPOOLDEFINE_H

#include <stdio.h>
#include <vector>

# ifdef linux
    #include <climits>    // linux environment INT_MAX is in this file
    #include <cstring>
# endif

const static int SLEEP_TIME_SECOND = INT_MAX;    // 单位是秒
const static int MAX_CHUNK_SIZE = INT_MAX;

struct FreeBlock {
    // 空闲内存，存放具体的节点信息
    explicit FreeBlock(unsigned int blockSize) {
        this->next = nullptr;
        this->data = new char[blockSize];
        memset(this->data, 0, blockSize);
    }

    ~FreeBlock() {
        if (this->data) {
            delete[] this->data;
            this->data = nullptr;
        }
    }

    char* data;
    FreeBlock *next;
};

struct MemChunk {
    // 内存块，每个chunk中包含了num个内存
    explicit MemChunk(unsigned int blockNumPerChunk, unsigned int blockSize) {
        blocks.reserve(blockNumPerChunk);
        for (unsigned int i = 0; i < blockNumPerChunk; i++) {
            auto *ptr = new FreeBlock(blockSize);
            blocks.push_back(ptr);
        }
        this->next = nullptr;
    }

    ~MemChunk() {
        for (auto& block : blocks) {
            if (block) {
                delete block;
                block = nullptr;
            }
            this->next = nullptr;
        }
    }

    std::vector<FreeBlock *> blocks;
    MemChunk *next;
};

#endif //CAISS_MEMORYPOOLDEFINE_H
