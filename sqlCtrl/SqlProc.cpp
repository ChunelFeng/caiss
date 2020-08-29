//
// Created by Chunel on 2020/8/30.
//

#include "SqlProc.h"

using namespace hsql;

SqlProc::SqlProc(){
    limit_num_ = DEFAULT_LIMIT_NUM;
    type_ = kStmtError;    // 初始化的时候，用个默认错误的占位
}

SqlProc::SqlProc(unsigned int limitNum){
    limit_num_ = limitNum;
    type_ = kStmtError;
}

SqlProc::~SqlProc()= default;

/**
 * 解析传入的sql指令
 * @param sql
 * @param sqlParams
 * @return
 */
CAISS_RET_TYPE SqlProc::parseSql(const char *sql, const void *sqlParams) {
    CAISS_FUNCTION_BEGIN
    CAISS_ASSERT_NOT_NULL(sql)

    SQLParserResult result;
    SQLParser::parse(sql, &result);
    if (!result.isValid()) {
        return CAISS_RET_SQL_PARSE;
    }

    std::vector<SQLStatement*> statements = result.getStatements();
    if (statements.empty()) {
        return CAISS_RET_SQL_PARSE;
    }

    auto* state = (SelectStatement *)statements[0];    // 通过以上两步判断，肯定能拿到第0个元素了
    table_name_ = (state->fromTable == nullptr) ? "" : state->fromTable->getName();
    limit_num_ = (unsigned int)((state->limit == nullptr) ? DEFAULT_LIMIT_NUM : state->limit->limit->ival);
    query_word_ = (nullptr == state->whereClause || nullptr == state->whereClause->expr2) ? "" : (state->whereClause->expr2->getName());
    type_ = state->type();

    CAISS_FUNCTION_END
}