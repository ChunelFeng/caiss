//
// Created by Chunel on 2020/7/10.
//

#include <string>
#include "TrieProc.h"

using namespace std;


/**
 * 插入信息
 * @param word
 */
void TrieProc::insert(const string& word) {
    innerInsert(getHeadNode(), word, 0);
}


/**
 * 查询信息
 * @param word
 * @return
 */
bool TrieProc::find(const string& word) {
    return innerFind(getHeadNode(), word, 0);
}


/**
 * 清空信息
 */
void TrieProc::clear() {
    innerClear(getHeadNode());
}


bool TrieProc::innerFind(TrieNode* node, const string& word, int index) {
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

void TrieProc::innerInsert(TrieNode* node, const string& word, int index) {
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


void TrieProc::innerClear(TrieNode* node) {
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


TrieNode* TrieProc::getHeadNode() {
    if (nullptr == head_) {
        lock_.writeLock();
        if (nullptr == head_) {
            head_ = new TrieNode("");
        }
        lock_.writeUnlock();
    }

    return head_;
}


/**
 * 获取字典树中，所有已经输入的单词
 * @return
 */
list<string> TrieProc::getAllWords() {
    list<string> words;
    innerGetWord(getHeadNode(), words);
    return words;
}


/**
 * 获取数上所有的词语信息
 * @param node
 * @param words
 */
void TrieProc::innerGetWord(TrieNode* node, list<string> &words) {
    if (nullptr == node) {
        return;
    }

    for (auto &cur : node->children) {
        if (cur) {
            if (cur->isEnd) {    // 如果是单词的话，则放入返回列表里
                words.push_back(cur->path);
            }
            innerGetWord(cur, words);
        }
    }
}


void TrieProc::eraser(const string &word) {
    bool isErased = false;    // 标识是否已经被忽略了
    innerEraser(getHeadNode(), word, 0, isErased);
}


/**
 * 忽略某个字符串
 * @param node
 * @param word
 * @param index
 * @param isErased 标记这个词已经被忽略，剪枝的作用
 */
void TrieProc::innerEraser(TrieNode *node, const string &word, int index, bool &isErased) {
    if (nullptr == node || isErased) {
        return;
    }

    if (word.size() == index && node->path == word && node->isEnd) {
        node->isEnd = false;    // 忽略当前词语
        isErased = true;
        return;
    }

    index++;
    for (auto &cur : node->children) {
        if (cur) {
            innerEraser(cur, word, index, isErased);
        }
    }
}
