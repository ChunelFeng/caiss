//
// Created by Chunel on 2020/7/1.
//

#ifndef CAISS_TRIEPROCDEFINE_H
#define CAISS_TRIEPROCDEFINE_H

#include <string>

const static unsigned int MAX_TRIE_NUM = 26;    // 仅针对小写字母

struct TrieNode {
    explicit TrieNode(const std::string& path) {
        this->isEnd = false;
        this->path = path;
        for (auto& i : children) {
            i = nullptr;    // 所有的都先赋值为空
        }
    }

    ~TrieNode() {
        for (auto& i : children) {
            if (i != nullptr) {
                delete i ;
                i  = nullptr;
            }
        }
    }

    bool isEnd;
    std::string path;    // 这个数据会增加数据的空间复杂度信息
    TrieNode* children[MAX_TRIE_NUM];
};

#endif //CAISS_TRIEPROCDEFINE_H
