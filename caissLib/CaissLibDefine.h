//
// Created by Chunel on 2020/5/23.
//

#ifndef _CHUNEL_CAISS_DEFINE_H_
#define _CHUNEL_CAISS_DEFINE_H_

#ifndef CAISS_LIB_API
    #if defined(_CNL_CAISS_EXPORTS)
        #define CAISS_LIB_API __declspec(dllexport)
    #else
        #define CAISS_LIB_API __declspec(dllimport)
    #endif
#endif

#ifdef WIN32
    #define STDCALL __stdcall
#else
    #define STDCALL
#endif

#define CAISS_VERSION       ("1.0.0")

#include <vector>

using CAISS_RET_TYPE = int;
using CAISS_UINT = unsigned int;
using CAISS_FLOAT = float;
using CAISS_BOOL = int;

using CAISS_VECTOR_FLOAT = std::vector<CAISS_FLOAT>;
using CAISS_VECTOR_UINT = std::vector<CAISS_UINT>;

typedef CAISS_FLOAT (STDCALL * CAISS_DIST_FUNC)(void *vec1, void *vec2, void* param);

/* 函数返回值定义 */
#define CAISS_RET_WARNING    (1)     // 流程告警
#define CAISS_RET_OK         (0)     // 流程正常
#define CAISS_RET_ERR        (-1)    // 流程异常
#define CAISS_RET_RES        (-2)    // 资源问题
#define CAISS_RET_MODE       (-3)    // 模式选择问题
#define CAISS_RET_PATH       (-4)    // 路径问题
#define CAISS_RET_JSON       (-5)    // json解析问题
#define CAISS_RET_PARAM      (-6)    // 参数问题
#define CAISS_RET_HANDLE     (-7)    // 句柄申请问题
#define CAISS_RET_DIM        (-8)    // 维度问题
#define CAISS_RET_MODEL_SIZE (-9)    // 模型尺寸限制问题
#define CAISS_RET_WORD_SIZE  (-10)   // 词语长度限制问题
#define CAISS_RET_NO_WORD    (-11)   // 词库中无对应词语问题

#define CAISS_TRUE       (1)
#define CAISS_FALSE      (0)

enum CAISS_MODE {
    CAISS_MODE_DEFAULT = 0,  // 无效模式
    CAISS_MODE_TRAIN = 1,    // 训练模式
    CAISS_MODE_PROCESS = 2,    // 处理模式
};

enum CAISS_SEARCH_TYPE {
    CAISS_SEARCH_DEFAULT = 1,
    CAISS_SEARCH_QUERY = 1,    // 通过快速检索的方式，查询query信息
    CAISS_SEARCH_WORD = 2,    // 通过快速检索的方式，查询word信息
    CAISS_LOOP_QUERY = 3,    // 通过暴力循环的方式，查询query信息
    CAISS_LOOP_WORD = 4    // 通过暴力循环的方式，查询word信息
};

enum CAISS_INSERT_TYPE {
    // 如果插入相同的数据
    CAISS_INSERT_DEFAULT = 1,
    CAISS_INSERT_OVERWRITE = 1,   // 覆盖原有的
    CAISS_INSERT_DISCARD = 2,    // 丢弃
};

enum CAISS_MANAGE_TYPE {
    CAISS_MANAGE_DEFAULT = 1,
    CAISS_MANAGE_SYNC = 1,    // 同步控制
    CAISS_MANAGE_ASYNC = 2,    // 异步控制
};

enum CAISS_DISTANCE_TYPE {
    CAISS_DISTANCE_DEFAULT = 1,
    CAISS_DISTANCE_EUC = 1,    // 欧氏距离
    CAISS_DISTANCE_INNER = 2,    // 内积距离
    CAISS_DISTANCE_JACCARD = 3,    // 杰卡德距离

    CAISS_DISTANCE_EDITION = 99,    // 自定义距离
};

enum CAISS_ALGO_TYPE {
    CAISS_ALGO_DEFAULT = 1,
    CAISS_ALGO_HNSW = 1,    // hnsw算法（准确度高，空间复杂度较大）
    CAISS_ALGO_NSG = 2    // nsg算法（准确度较高，空间复杂度小）
};


#endif //_CHUNEL_CAISS_DEFINE_H_
