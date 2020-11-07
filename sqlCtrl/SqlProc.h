//
// Created by Chunel on 2020/8/30.
//

#ifndef CAISS_SQLCTRL_H
#define CAISS_SQLCTRL_H

#include <vector>

#include "../caissLib/CaissLibDefine.h"
#include "../utilsCtrl/UtilsDefine.h"

#include "util/SQLParser.h"
#include "util/SQLParserResult.h"
#include "SqlProcDefine.h"


using namespace hsql;

const static unsigned int DEFAULT_LIMIT_NUM = 5;

class SqlProc {
public:
    explicit SqlProc(unsigned int limitNum);
    explicit SqlProc();
    virtual ~SqlProc();

    CAISS_STATUS parseSql(const char *sql,
                          const void *sqlParams = nullptr);

    const char *getQueryWord() const {
        return search_info_.c_str();
    }

    unsigned int getLimitNum() const {
        return search_limit_num_;
    }

    const std::string &getTableName() const {
        return table_name_;
    }

    hsql::StatementType getType() const {
        return type_;
    }

    const std::vector<std::string> &getInsertKeys() const {
        return insert_keys_;
    }

    const std::vector<void *>& getInsertValues() const {
        return insert_values_;
    }

    const char* getDeleteQueryWord() const {
        return delete_info_.c_str();
    }

    const char* getUpdateInfo() const {
        return update_info_.c_str();
    }

    void *getUpdateValue() const {
        return update_value_;
    }

protected:
    int parseInsertSql(SQLStatement *state);    // 插入语句解析（支持多条插入）
    int parseSearchSql(SQLStatement *state);    // 查询语句解析
    int parseDeleteSql(SQLStatement *state);    // 删除语句解析
    int parseUpdateSql(SQLStatement *state);    // 修改语句解析

private:
    std::string table_name_;
    hsql::StatementType type_;
    CAISS_SQL_TYPE sql_type_;    // 用于保留是根据word查询，还是根据sentence查询的方法

    // select 对应的信息
    std::string search_info_;
    unsigned int search_limit_num_;

    // insert 对应的信息
    std::vector<std::string> insert_keys_;
    std::vector<void *> insert_values_;    // 传入的是待插入的指针位置

    // delete 对应的信息
    std::string delete_info_;

    // update 对应的信息
    std::string update_info_;
    void *update_value_;
};

#endif //CAISS_SQLCTRL_H
