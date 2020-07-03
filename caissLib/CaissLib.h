/**************************************************************
* 当前版本：1.2.1
* 作    者: Chunel
*　　　　　　　　┏┓　 ┏┓+ +
*　　　　　　　┏┛┻━━━┛┻┓ + +
*　　　　　　　┃　　　　　　　┃
*　　　　　　　┃　　　━　　　┃ ++ + + +
*　　　　　　 ████━████ ┃+
*　　　　　　　┃　　　　　　　┃ +
*　　　　　　　┃　　　┻　　　┃
*　　　　　　　┃　　　　　　　┃ + +
*　　　　　　　┗━┓　　　┏━┛
*　　　　　　　　　┃　　　┃
*　　　　　　　　　┃　　　┃ + + + +
*　　　　　　　　　┃　　　┃　　　　Code is far away from bug with the animal protecting
*　　　　　　　　　┃　　　┃ + 　　　　神兽保佑,代码无bug
*　　　　　　　　　┃　　　┃
*　　　　　　　　　┃　　　┃　　+
*　　　　　　　　　┃　 　 ┗━━━┓++
*　　　　　　　　　┃ 　　　　　　　┣┓
*　　　　　　　　　┃ 　　　　　　　┏┛
*　　　　　　　　　┗┓┓┏━┳┓┏┛ + + + +
*　　　　　　　　　　┃┫┫　┃┫┫
*　　　　　　　　　　┗┻┛　┗┻┛+ + + +
*
* ━━━━━━感觉萌萌哒━━━━━━
**************************************************************/

#ifndef _CHUNEL_CAISS_LIBRARY_H_
#define _CHUNEL_CAISS_LIBRARY_H_

#include "CaissLibDefine.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

    /**
     * 初始化环境信息
     * @param maxThreadSize 支持的最大并发数
     * @param algoType 算法类型（详见CaissLibDefine.h文件）
     * @param manageType 并发类型（详见CaissLibDefine.h文件）
     * @return 运行成功返回0，警告返回1，其他异常值，参考错误码定义
     */
    CAISS_LIB_API CAISS_RET_TYPE STDCALL CAISS_Environment(const unsigned int maxThreadSize,
            const CAISS_ALGO_TYPE algoType,
            const CAISS_MANAGE_TYPE manageType);

    /**
     * 创建句柄信息
     * @param handle 句柄信息
     * @return 运行成功返回0，警告返回1，其他异常值，参考错误码定义
     */
    CAISS_LIB_API CAISS_RET_TYPE STDCALL CAISS_CreateHandle(void** handle);

    /**
     * 初始化信息
     * @param handle 句柄信息
     * @param mode 处理类型（详见CaissLibDefine.h文件）
     * @param distanceType 距离类型（详见CaissLibDefine.h文件）
     * @param dim 维度
     * @param modelPath 模型路径
     * @param distFunc 距离计算函数（仅针对自定义距离计算生效）
     * @return 运行成功返回0，警告返回1，其他异常值，参考错误码定义
     */
    CAISS_LIB_API CAISS_RET_TYPE STDCALL CAISS_Init(void *handle,
            const CAISS_MODE mode,
            const CAISS_DISTANCE_TYPE distanceType,
            const unsigned int dim,
            const char *modelPath,
            const CAISS_DIST_FUNC distFunc);

    /**
     * 模型训练功能 （当快速查询fastRank个数，均在真实realRank个数的范围内的准确率，超过precision的时候，训练完成）
     * @param handle 句柄信息
     * @param dataPath 带训练样本路径（训练文件格式，参考说明文档）
     * @param maxDataSize 最大样本个数
     * @param normalize 样本数据是否归一化
     * @param maxIndexSize 样本标签最大长度
     * @param precision 目标精确度
     * @param fastRank 快速查询排名个数
     * @param realRank 真实查询排名个数
     * @param step 迭代步径
     * @param maxEpoch 最大迭代轮数 （maxEpoch轮后，准确率仍不满足要求，则停止训练，返回警告信息）
     * @param showSpan 信息打印行数
     * @return 运行成功返回0，警告返回1，其他异常值，参考错误码定义
     * @notice 训练文件格式，参考doc文件夹下内容
     */
    CAISS_LIB_API CAISS_RET_TYPE STDCALL CAISS_Train(void *handle,
            const char *dataPath,
            const unsigned int maxDataSize,
            const CAISS_BOOL normalize,
            const unsigned int maxIndexSize,
            const float precision,
            const unsigned int fastRank,
            const unsigned int realRank,
            const unsigned int step,
            const unsigned int maxEpoch,
            const unsigned int showSpan);


    /**
     * 查询功能
     * @param handle 句柄信息
     * @param info 待查询的信息
     * @param searchType 查询信息的类型（详见CaissLibDefine.h文件）
     * @param topK 返回最近的topK个信息
     * @param filterEditDistance 需要过滤的最小词语编辑距离（仅针对根据单词查询的情况下生效，-1表示不过滤，0表示过滤跟）
     * @param searchCBFunc 查询到结果后，执行回调函数，传入的是查询到结果的word信息和distance信息
     * @param cbParams 回调函数中，传入的参数信息
     * @return 运行成功返回0，警告返回1，其他异常值，参考错误码定义
     * @notice filterEditDistance仅针对根据单词过滤的情况下生效。
     *         =-1表示不过滤；=0表示过滤跟当前词语完全相同的；
     *         =3表示过滤跟当前词语相编辑距离的在3以内的，以此类推；
     *         最大值不超过CAISS_MAX_EDIT_DISTANCE值
     */
    CAISS_LIB_API CAISS_RET_TYPE STDCALL CAISS_Search(void *handle,
            void *info,
            const CAISS_SEARCH_TYPE searchType,
            const unsigned int topK,
            const unsigned int filterEditDistance = CAISS_DEFAULT_EDIT_DISTANCE,
            const CAISS_SEARCH_CALLBACK searchCBFunc = nullptr,
            const void *cbParams = nullptr);


    /**
     * 获取结果字符串长度
     * @param handle 句柄信息
     * @param size 结果长度
     * @return 运行成功返回0，警告返回1，其他异常值，参考错误码定义
     */
    CAISS_LIB_API CAISS_RET_TYPE STDCALL CAISS_getResultSize(void *handle,
            unsigned int &size);

    /**
     * 获取查询结果信息
     * @param handle 句柄信息
     * @param result 结果信息
     * @param size 对应结果长度
     * @return 运行成功返回0，警告返回1，其他异常值，参考错误码定义
     */
    CAISS_LIB_API CAISS_RET_TYPE STDCALL CAISS_getResult(void *handle,
            char *result,
            unsigned int size);

    /**
     * 插入信息
     * @param handle 句柄信息
     * @param node 待插入的向量信息
     * @param label 待插入向量的标签信息
     * @param insertType 插入类型（详见CaissLibDefine.h文件）
     * @return 运行成功返回0，警告返回1，其他异常值，参考错误码定义
     * @notice 插入信息实时生效。程序结束后，是否保存新插入的信息，取决于是否调用CAISS_Save()方法
     */
    CAISS_LIB_API CAISS_RET_TYPE STDCALL CAISS_Insert(void *handle,
            CAISS_FLOAT *node,
            const char *label,
            CAISS_INSERT_TYPE insertType);

    /**
     * 保存模型
     * @param handle 句柄信息
     * @param modelPath 模型保存路径（默认值是覆盖当前模型）
     * @return 运行成功返回0，警告返回1，其他异常值，参考错误码定义
     */
    CAISS_LIB_API CAISS_RET_TYPE STDCALL CAISS_Save(void *handle,
            const char *modelPath = nullptr);

    /**
     * 销毁句柄信息
     * @param handle 句柄信息
     * @return 运行成功返回0，警告返回1，其他异常值，参考错误码定义
     */
    CAISS_LIB_API CAISS_RET_TYPE STDCALL CAISS_destroyHandle(void *handle);

#ifdef __cplusplus
}
#endif    // __cplusplus

#endif //_CHUNEL_CAISS_LIBRARY_H_
