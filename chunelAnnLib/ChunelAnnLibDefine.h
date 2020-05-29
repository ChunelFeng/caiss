//
// Created by Chunel on 2020/5/23.
//

#ifndef _CHUNEL_ANN_DEFINE_H_
#define _CHUNEL_ANN_DEFINE_H_

#include <vector>

#define ANN_VERSION       ("1.0.0")

using ANN_RET_TYPE = int;
using ANN_UINT = unsigned int;
using ANN_FLOAT = float;
using ANN_BOOL = int;

using ANN_VECTOR_FLOAT = std::vector<ANN_FLOAT>;
using ANN_VECTOR_UINT = std::vector<ANN_UINT>;

/* 函数返回值定义 */
#define ANN_RET_OK         (0)
#define ANN_RET_ERR        (-1)
#define ANN_RET_RES        (-2)
#define ANN_RET_MODE       (-3)
#define ANN_RET_PATH       (-4)
#define ANN_RET_JSON       (-5)
#define ANN_RET_PARAM      (-6)
#define ANN_RET_HANDLE     (-7)
#define ANN_RET_DIM        (-8)

#define ANN_TRUE       (1)
#define ANN_FALSE      (0)

enum ANN_MODE {
    ANN_MODE_DEFAULT = 0,  // 默认模式
    ANN_MODE_TRAIN = 1,    // 训练模式
    ANN_MODE_PROCESS,    // 处理模式
    ANN_MODE_UPDATE,    // 更新模式
};

enum ANN_SEARCH_TYPE {
    ANN_SEARCH_DEFAULT = 0,
    ANN_SEARCH_FAST = 0,    // 快速查询
    ANN_SEARCH_REAL = 1,    // 循环查询，保证结果准确
};

enum ANN_INSERT_TYPE {
    // 如果插入相同的数据
    ANN_INSERT_DEFAULT = 0,
    ANN_INSERT_ADD = 0,   // 在后面添加
    ANN_INSERT_OVERWRITE = 1,   // 覆盖原有的
    ANN_INSERT_DISCARD = 2,    // 丢弃
};

enum ANN_MANAGE_TYPE {
    ANN_MANAGE_SYNC = 0,    // 同步控制
    ANN_MANAGE_ASYNC = 1,    // 异步控制
};

enum ANN_DISTANCE_TYPE {
    ANN_DISTANCE_EUC = 0,    // 欧氏距离
    ANN_DISTANCE_INNER = 1,    // cos距离

    ANN_DISTANCE_EDITION = 99,    // 自定义距离
};


#endif //_CHUNEL_ANN_DEFINE_H_
