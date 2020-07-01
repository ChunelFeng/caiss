//
// Created by Chunel on 2020/7/1.
//

#include <iostream>
#include <string>
#include "./trieProc/TrieProc.h"

using namespace std;

int main() {
    TrieProc* trie = new TrieProc();

    string words[] = {"ab"};
    //string words[4] = {"ab"};
    for (auto &word : words) {
        trie->insert(word);
    }

    bool ret = trie->find("ab");
    cout << ret << endl;

    trie->clear();

    return 0;
}