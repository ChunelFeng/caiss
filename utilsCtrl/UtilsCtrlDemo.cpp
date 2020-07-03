//
// Created by Chunel on 2020/7/1.
//

#include <iostream>
#include <string>
#include "UtilsInclude.h"

using namespace std;

int demo_Trie () {
    auto* trie = new TrieProc();

    string words[] = {"ab"};
    //string words[4] = {"ab"};
    for (auto &word : words) {
        trie->insert(word);
    }

    bool ret = trie->find("ab");
    cout << ret << endl;

    trie->clear();
    delete trie;

    return 0;
}

int demo_EditDistanceProc() {
    string a = "help";
    string b = "helapp";
    int i = EditDistanceProc::calc(a, b);
    cout << i << endl;
    return 0;
}

int main() {
    demo_EditDistanceProc();
    return 0;
}