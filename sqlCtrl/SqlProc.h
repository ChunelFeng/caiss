//
// Created by Chunel on 2020/8/30.
//

#ifndef CAISS_SQLCTRL_H
#define CAISS_SQLCTRL_H

#include "../caissLib/CaissLibDefine.h"
#include "../utilsCtrl/UtilsDefine.h"

#include "util/SQLParser.h"
#include "util/SQLParserResult.h"

const static unsigned int DEFAULT_LIMIT_NUM = 5;

class SqlProc {
public:
    explicit SqlProc(unsigned int limitNum);
    explicit SqlProc();
    virtual ~SqlProc();

    CAISS_RET_TYPE parseSql(const char *sql,
            const void *sqlParams = nullptr);

    std::string getQueryWord() const {
        return query_word_;
    }

    unsigned int getLimitNum() const {
        return limit_num_;
    }

    std::string getTableName() const {
        return table_name_;
    }

    hsql::StatementType getType() const {
        return type_;
    }

private:
    std::string query_word_;
    unsigned int limit_num_;
    std::string table_name_;
    hsql::StatementType type_;
};


#endif //CAISS_SQLCTRL_H
