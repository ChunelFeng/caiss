//
// Created by Chunel on 2020/5/23.
//

#ifndef _CHUNEL_ANN_DEFINE_H_
#define _CHUNEL_ANN_DEFINE_H_

#ifndef ANN_LIB_API
    #if defined(_CNL_ANN_EXPORTS)
        #define ANN_LIB_API __declspec(dllexport)
    #else
        #define ANN_LIB_API __declspec(dllimport)
    #endif
#endif

#ifdef WIN32
    #define STDCALL __stdcall
#else
    #define STDCALL
#endif

#define ANN_VERSION       ("1.0.0")

#include <vector>

using ANN_RET_TYPE = int;
using ANN_UINT = unsigned int;
using ANN_FLOAT = float;
using ANN_BOOL = int;

using ANN_VECTOR_FLOAT = std::vector<ANN_FLOAT>;
using ANN_VECTOR_UINT = std::vector<ANN_UINT>;

typedef ANN_FLOAT (STDCALL * CAISS_DIST_FUNC)(const void *pVect1, const void *pVect2, const void *qty_ptr);


/* 函数返回值定义 */
#define ANN_RET_WARNING    (1)     // 流程告警
#define ANN_RET_OK         (0)     // 流程正常
#define ANN_RET_ERR        (-1)    // 流程异常
#define ANN_RET_RES        (-2)    // 资源问题
#define ANN_RET_MODE       (-3)    // 模式选择问题
#define ANN_RET_PATH       (-4)    // 路径问题
#define ANN_RET_JSON       (-5)    // json解析问题
#define ANN_RET_PARAM      (-6)    // 参数问题
#define ANN_RET_HANDLE     (-7)    // 句柄申请问题
#define ANN_RET_DIM        (-8)    // 维度问题
#define ANN_RET_MODEL_SIZE (-9)    // 模型尺寸限制问题
#define ANN_RET_WORD_SIZE  (-10)   // 词语长度限制问题
#define ANN_RET_NO_WORD    (-11)   // 词库中无对应词语问题

#define ANN_TRUE       (1)
#define ANN_FALSE      (0)

enum ANN_MODE {
    ANN_MODE_DEFAULT = 0,  // 无效模式
    ANN_MODE_TRAIN = 1,    // 训练模式
    ANN_MODE_PROCESS = 2,    // 处理模式
};

enum ANN_SEARCH_TYPE {
    ANN_SEARCH_DEFAULT = 1,
    ANN_SEARCH_QUERY = 1,    // 通过query信息查询
    ANN_SEARCH_WORD = 2,    // 通过word信息查询
};

enum ANN_INSERT_TYPE {
    // 如果插入相同的数据
    ANN_INSERT_DEFAULT = 1,
    ANN_INSERT_OVERWRITE = 1,   // 覆盖原有的
    ANN_INSERT_DISCARD = 2,    // 丢弃
};

enum ANN_MANAGE_TYPE {
    ANN_MANAGE_DEFAULT = 1,
    ANN_MANAGE_SYNC = 1,    // 同步控制
    ANN_MANAGE_ASYNC = 2,    // 异步控制
};

enum ANN_DISTANCE_TYPE {
    ANN_DISTANCE_DEFAULT = 1,
    ANN_DISTANCE_EUC = 1,    // 欧氏距离
    ANN_DISTANCE_INNER = 2,    // cos距离

    ANN_DISTANCE_EDITION = 99,    // 自定义距离
};

enum ANN_ALGO_TYPE {
    ANN_ALGO_DEFAULT = 1,
    ANN_ALGO_HNSW = 1,    // hnsw算法（准确度高，空间复杂度较大）
    ANN_ALGO_NSG = 2    // nsg算法（准确度较高，空间复杂度小）
};


#endif //_CHUNEL_ANN_DEFINE_H_
