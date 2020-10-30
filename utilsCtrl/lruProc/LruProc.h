//
// Created by Chunel on 2020/6/16.
//

#ifndef CAISS_LRUPROC_H
#define CAISS_LRUPROC_H

#include <string>
#include <list>
#include <unordered_map>
#include <utility>

#include "./LruProcDefine.h"
#include "../UtilsProc.h"

using namespace std;

class LruProc : public UtilsProc {
public:
    explicit LruProc(unsigned int capacity) {
        this->cur_size_ = 0;
        this->capacity_ = capacity;
    };

    explicit LruProc() {
        this->cur_size_ = 0;
        this->capacity_ = DEFAULT_CAPACITY;
    };

    ~LruProc() override {
        this->clear();
    }

    /**
     * 获取数据
     * @param word
     * @return
     */
    ALOG_RET_TYPE get(const string& word) {
        ALOG_RET_TYPE result;
        auto cur = cache_.find(word);
        if (cur != cache_.end()) {
            result = cur->second->result;    // 找到的情况
            put(word, result);    // 重新放入，就是为了靠前一些
        }

        return result;
    };

    /**
     * 插入数据
     * @param word
     * @param result
     * @return
     */
    int put(const string& word, const ALOG_RET_TYPE& result) {
        auto cur = cache_.find(word);
        if (cur != cache_.end()) {
            nodes_.erase(cache_[word]);    // 删除对应的iter信息
            cache_.erase(word);    // 如果存在，先删掉，后来加上
            cur_size_--;
        } else if (cur_size_ >= capacity_) {
            auto back = nodes_.back();    // 删除最后一个
            cache_.erase(back.word);
            nodes_.pop_back();    // 如果不存在，并且已经超过容量了，则删除最后一个
            cur_size_--;
        }

        nodes_.push_front(LruNode(word, result));
        cache_[word] = nodes_.begin();
        cur_size_++;

        return 0;
    };

    /**
     * 清空缓存
     * @return
     */
    int clear() {    // 清空内部的所有数据
        nodes_.clear();
        cache_.clear();
        cur_size_ = 0;    // 当前数量清零，容量保持不变

        return 0;
    }


private:
    unsigned int cur_size_;
    unsigned int capacity_;
    unordered_map<string, list<LruNode>::iterator> cache_;    // 缓存的信息，保存list中的
    list<LruNode> nodes_;
};


#endif //CAISS_LRUPROC_H
