//
// Created by Chunel on 2020/7/10.
//

#include <iostream>
#include <list>
#include "UtilsInclude.h"


using namespace std;


int main() {
    string str[4] = {"abc", "abd", "def", "abcdef"};
    auto* ptr = new TrieProc();
    for (auto &s : str) {
        ptr->insert(s);
    }

    ptr->eraser("ab");

    list<string> mm = ptr->getAllWords();
    for (auto &m : mm) {
        cout << m << endl;
    }

    return 0;
}