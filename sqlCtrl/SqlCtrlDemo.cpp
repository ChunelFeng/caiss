//
// Created by Chunel on 2020/8/29.
//

#include <iostream>
#include "SqlProc.h"
//#include "util/SQLParser.h"
//#include "util/SQLParserResult.h"

using namespace std;
using namespace hsql;

int main() {
    const std::string query = "select * from tableA where word like hello limit 10";
//    SQLParserResult result;
//    SQLParser::parse(query, &result);
    auto *proc = new SqlProc(5);
    proc->parseSql(query.c_str(), nullptr);

    return 0;
}