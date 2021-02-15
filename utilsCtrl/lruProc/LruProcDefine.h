//
// Created by Chunel on 2020/6/16.
//

#ifndef CAISS_LRUPROCDEFINE_H
#define CAISS_LRUPROCDEFINE_H

#include <string>
#include <utility>
using namespace std;

const static unsigned int DEFAULT_CAPACITY = 10;

template<typename T>
struct LruNode {
public:
    string word;    // 被查询的单词
    T result;

    LruNode(const string &word, const T &result) {
        this->word = word;
        this->result = result;
    }
};

#endif //CAISS_LRUPROCDEFINE_H
