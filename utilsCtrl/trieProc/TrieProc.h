//
// Created by Chunel on 2020/7/1.
//

#ifndef CAISS_TRIEPROC_H
#define CAISS_TRIEPROC_H


#include <string>
#include "../UtilsProc.h"
#include "TrieProcDefine.h"

using namespace std;

class TrieProc : public UtilsProc {
public:
    TrieProc() {
        this->head_ = new TrieNode("");    // 总的头结点
    }

    ~TrieProc() override {
        this->clear();    // 切记，head_不能删除
    }


    void insert(const string& word) {
        this->innerInsert(head_, word, 0);
    }

    bool find(const string& word) {
        return this->innerFind(head_, word, 0);
    }

    void clear() {
        this->innerClear(head_);
    }

protected:

    bool innerFind(TrieNode* node, const string& word, int index) {
        if (nullptr == node) {
            return false;
        }

        int i = word[index] - 'a';    // 第i个字母（a为第0个）
        if (index == word.size()) {
            return node->isEnd;    // 如果是达到长度了，则返回这个节点是不是单词
        }

        bool ret = false;
        if (node->children[i]) {
            ret = innerFind(node->children[i], word, ++index);
        }

        return ret;
    }


    void innerInsert(TrieNode* node, const string& word, int index) {
        if (nullptr == node) {
            return;
        }

        if (index == word.size()) {
            node->isEnd = true;
            return;
        }

        int i = word[index] - 'a';
        ++index;
        if (!node->children[i]) {
            // 如果node为空的话
            string curPath = word.substr(0, index);
            node->children[i] = new TrieNode(curPath);
        }

        innerInsert(node->children[i], word, index);
    }

    void innerClear(TrieNode* node) {
        if (nullptr == node) {
            return;
        }

        for (auto &cur : node->children) {
            if (cur) {
                innerClear(cur);
                delete cur;
                cur = nullptr;
            }
        }
    }

private:
    TrieNode* head_;
};


#endif //CAISS_TRIEPROC_H
