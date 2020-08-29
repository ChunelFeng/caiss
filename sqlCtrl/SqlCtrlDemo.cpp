//
// Created by Chunel on 2020/8/29.
//

#include <iostream>
#include "SqlProc.h"

using namespace std;
using namespace hsql;

int main() {
    const std::string query = "select * from tableA where word like hello limit 10";

    auto *proc = new SqlProc(5);
    proc->parseSql(query.c_str(), nullptr);

    return 0;
}