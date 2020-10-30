//
// Created by Chunel on 2020/8/30.
//

#include "SqlProc.h"

SqlProc::SqlProc(){
    search_limit_num_ = DEFAULT_LIMIT_NUM;
    type_ = kStmtError;    // 初始化的时候，用个默认错误的占位
}

SqlProc::SqlProc(unsigned int limitNum){
    search_limit_num_ = limitNum;
    type_ = kStmtError;
}

SqlProc::~SqlProc()= default;

/**
 * 解析传入的sql指令
 * @param sql
 * @param sqlParams
 * @return
 */
CAISS_STATUS SqlProc::parseSql(const char *sql, const void *sqlParams) {
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

    type_ = statements[0]->type();
    switch (type_) {
        case kStmtInsert:
            ret = parseInsertSql(statements[0]);
            break;
        case kStmtSelect:
            ret = parseSearchSql(statements[0]);
            break;
        case kStmtDelete:
            ret = parseDeleteSql(statements[0]);
            break;
        case kStmtUpdate:
            ret = parseUpdateSql(statements[0]);
            break;
        default:
            break;
    }
    CAISS_FUNCTION_CHECK_STATUS

    CAISS_FUNCTION_END
}

int SqlProc::parseInsertSql(SQLStatement *state) {
    CAISS_FUNCTION_BEGIN
    CAISS_ASSERT_NOT_NULL(state)

    auto *insertState = (InsertStatement *)state;
    int size = (int)insertState->columns->size();
    for (int i = 0; i < size; ++i) {
        insert_keys_.emplace_back((*insertState->columns)[i]);
        insert_values_.emplace_back((void *)((*insertState->values)[i]->ival));    // 这种是说明传入的是地址信息，地址信息被翻译成long long类型
    }
    table_name_ = insertState->tableName;

    CAISS_FUNCTION_END
}


int SqlProc::parseSearchSql(SQLStatement *state) {
    CAISS_FUNCTION_BEGIN
    CAISS_ASSERT_NOT_NULL(state)

    auto *selectState = (SelectStatement *)state;

    search_limit_num_ = (unsigned int)((selectState->limit == nullptr) ? DEFAULT_LIMIT_NUM : selectState->limit->limit->ival);
    search_info_ = (nullptr == selectState->whereClause || nullptr == selectState->whereClause->expr2)
            ? "" : (selectState->whereClause->expr2->getName());
    table_name_ = (selectState->fromTable == nullptr) ? "" : selectState->fromTable->getName();    // todo 看看这里能不能优化掉
    CAISS_FUNCTION_END
}

int SqlProc::parseDeleteSql(SQLStatement *state) {
    CAISS_FUNCTION_BEGIN
    CAISS_ASSERT_NOT_NULL(state)

    auto *deleteState = (DeleteStatement *)state;
    delete_info_ = (deleteState->expr != nullptr && deleteState->expr->expr2 != nullptr)
            ? deleteState->expr->expr2->getName()
            : "";
    table_name_ = deleteState->tableName;

    CAISS_FUNCTION_END
}

int SqlProc::parseUpdateSql(SQLStatement *state) {
    CAISS_FUNCTION_BEGIN
    CAISS_ASSERT_NOT_NULL(state)

    auto *updateState = (UpdateStatement *)state;
    update_info_ = (updateState->where && updateState->where->expr2 && updateState->where->expr2->name)
            ? updateState->where->expr2->name : "";
    update_value_ = (void *)((*updateState->updates)[0]->value->ival);
    table_name_ = updateState->table->getName();

    CAISS_FUNCTION_END
}