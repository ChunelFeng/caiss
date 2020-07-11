//
// Created by Chunel on 2020/7/1.
//

#ifndef CAISS_TRIEPROC_H
#define CAISS_TRIEPROC_H


#include <string>
#include <list>
#include "../UtilsProc.h"
#include "TrieProcDefine.h"
#include "../../threadCtrl/ThreadInclude.h"

using namespace std;

class TrieProc : public UtilsProc {
public:
    TrieProc() {
        this->head_ = nullptr;
        getHeadNode();    // 构造的时候，将head_信息初始化
    }

    ~TrieProc() override {
        clear();    // 切记，head_不能删除
    }

    /**
     * 插入信息
     * @param word
     */
    void insert(const string& word);

    /**
     * 查询信息
     * @param word
     * @return
     */
    bool find(const string& word);

    /**
     * 清空信息
     */
    void clear();

    /**
     * 忽略这个节点
     * @param word
     */
    void eraser(const string& word);

    /**
     * 获取所有的节点信息
     * @return
     */
    list<string> getAllWords();


protected:
    bool innerFind(TrieNode* node, const string& word, int index);
    void innerInsert(TrieNode* node, const string& word, int index);
    void innerClear(TrieNode* node);
    TrieNode* getHeadNode();
    void innerEraser(TrieNode *node, const string &word, int index, bool &isErased);

private:
    TrieNode* head_;
    RWLock lock_;
};


#endif //CAISS_TRIEPROC_H
