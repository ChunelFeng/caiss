//
// Created by Chunel on 2020/9/12.
//

#ifndef CAISS_SQLPROCDEFINE_H
#define CAISS_SQLPROCDEFINE_H

enum CAISS_SQL_TYPE {
    // sql语句的意图类型
    CAISS_SQL_TYPE_DEFAULT = 0,
    CAISS_SQL_TYPE_WORD = 1,          // 单词处理方式
    CAISS_SQL_TYPE_SENTENCE = 2       // 句子处理方式
};

#endif //CAISS_SQLPROCDEFINE_H
