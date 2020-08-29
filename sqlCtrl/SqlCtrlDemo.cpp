//
// Created by Chunel on 2020/8/29.
//

#include <iostream>
#include "util/SQLParser.h"
#include "util/SQLParserResult.h"

using namespace std;
using namespace hsql;

int main() {
    const std::string query = "select * from tableA where 'word' = hello limit 10";
    hsql::SQLParserResult result;
    hsql::SQLParser::parse(query, &result);

    return 0;
}