//
// Created by Chunel on 2020/6/16.
//

#ifndef CAISS_LRUPROCDEFINE_H
#define CAISS_LRUPROCDEFINE_H

#include <string>
#include <utility>
using namespace std;

const static unsigned int DEFAULT_CAPACITY = 5;

struct LruNode {
    string word;    // 被查询的单词
    string result;    // 对应的结果信息

    LruNode(string word, string result) {
        this->word = std::move(word);
        this->result = std::move(result);
    }
};

#endif //CAISS_LRUPROCDEFINE_H
