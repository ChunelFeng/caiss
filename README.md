# <center> Caiss库使用说明 </center>

## 1. 简介

&ensp;&ensp;&ensp;&ensp; 随着人工智能技术的普及，海量高维度向量的相似度查询技术在研究和生产中的作用与日俱增。目前，市面有许多优秀开源的解决方案，但是，在使用过程中，遇到了一些问题。比如，
* 由于对于各种算法原理的了解不深，不会配置参数，导致的训练模型结果偏差较大。
* 开源库对于各种距离的支持有限，无法满足随时变化的实验需求。
* 标签信息和向量的分离，导致标定和查询需要在不同的步骤中完成。
* 部分解决方案，对于平台或者对于编程语言的依赖，导致了各种环境问题。

&ensp;&ensp;&ensp;&ensp; 在这里，我们基于Google，Facebook，阿里巴巴等互联网巨头的现有成果，实现了一套全新思路开源的解决方案。提供面向最终结果的训练过程，会在训练过程中，根据设定的目标，自动调节参数。提供自定义距离的训练和查询方式。支持训练过程中，标签信息和向量信息的绑定。提供纯C风格的SDK（注：编译依赖boost1.67.0或以上版本），支持Windows，Linux和Mac系统，并方便迁移到其他编程语言，如python，java等。

* &ensp;&ensp;&ensp;&ensp; 我们把这个库，命名为Caiss (Chunel Artificial Intelligence Similarity Search)，希望可以在大家的研究和生产过程中，发挥积极的作用。


## 2. 相关信息定义

```cpp
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

```

## 3. 相关接口定义

```cpp

    /**
     * 初始化环境信息
     * @param maxSize 支持的最大并发数
     * @param algoType 算法类型（详见CaissLibDefine.h文件）
     * @param manageType 并发类型（详见CaissLibDefine.h文件）
     * @return 运行成功返回0，警告返回1，其他异常值，参考错误码定义
     */
   CAISS_RET_TYPE CAISS_Environment(unsigned int maxSize,
            const CAISS_ALGO_TYPE algoType,
            const CAISS_MANAGE_TYPE manageType);

    /**
     * 创建句柄信息
     * @param handle 句柄信息
     * @return 运行成功返回0，警告返回1，其他异常值，参考错误码定义
     */
   CAISS_RET_TYPE CAISS_CreateHandle(void** handle);

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
   CAISS_RET_TYPE CAISS_Init(void *handle,
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
     * @notice 训练文件格式，参考说明文档
     */
   CAISS_RET_TYPE CAISS_Train(void *handle,
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
     * @return 运行成功返回0，警告返回1，其他异常值，参考错误码定义
     */
   CAISS_RET_TYPE CAISS_Search(void *handle,
            void *info,
            const CAISS_SEARCH_TYPE searchType,
            const unsigned int topK);

    /**
     * 获取结果字符串长度
     * @param handle 句柄信息
     * @param size 结果长度
     * @return 运行成功返回0，警告返回1，其他异常值，参考错误码定义
     */
   CAISS_RET_TYPE CAISS_getResultSize(void *handle,
            unsigned int &size);

    /**
     * 获取查询结果信息
     * @param handle 句柄信息
     * @param result 结果信息
     * @param size 对应结果长度
     * @return 运行成功返回0，警告返回1，其他异常值，参考错误码定义
     */
   CAISS_RET_TYPE CAISS_getResult(void *handle,
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
   CAISS_RET_TYPE CAISS_Insert(void *handle,
            CAISS_FLOAT *node,
            const char *label,
            CAISS_INSERT_TYPE insertType);

    /**
     * 保存模型
     * @param handle 句柄信息
     * @param modelPath 模型保存路径（默认值是覆盖当前模型）
     * @return 运行成功返回0，警告返回1，其他异常值，参考错误码定义
     */
   CAISS_RET_TYPE CAISS_Save(void *handle,
            char *modelPath);

    /**
     * 销毁句柄信息
     * @param handle 句柄信息
     * @return 运行成功返回0，警告返回1，其他异常值，参考错误码定义
     */
   CAISS_RET_TYPE CAISS_destroyHandle(void *handle);


```

## 4. 使用Demo

```cpp

#include "CaissLib.h"
#include <iostream>
#include <string>


using namespace std;

static const unsigned int max_thread_num_ = 4;
static const CAISS_ALGO_TYPE algo_type_ = CAISS_ALGO_HNSW;
static const CAISS_MANAGE_TYPE manage_type_ = CAISS_MANAGE_SYNC;
static const CAISS_MODE mode_ = CAISS_MODE_PROCESS;
static const CAISS_DISTANCE_TYPE dist_type_ = CAISS_DISTANCE_INNER;
static const unsigned int dim_ = 768;
static const char *model_path_ = "libDemo.caiss";
static const CAISS_DIST_FUNC dist_func_ = nullptr;
static std::string info_ = "hello";
static const CAISS_SEARCH_TYPE search_type_ = CAISS_SEARCH_WORD;
static const unsigned int top_k_ = 5;

static const string data_path_ = "bert_71290words_768dim.txt";
static const unsigned int max_data_size_ = 100000;
static const CAISS_BOOL normalize_ = CAISS_TRUE;
static const unsigned int max_index_size_ = 64;
static const float precision_ = 0.95;
static const unsigned int fast_rank_ = 5;
static const unsigned int real_rank_ = 5;
static const unsigned int step_ = 1;
static const unsigned int max_epoch_ = 3;
static const unsigned int show_span_ = 1000;

static int train() {
    /* 训练功能 */
    int ret = CAISS_RET_OK;

    void *handle = nullptr;
    ret = CAISS_CreateHandle(&handle);

    ret = CAISS_Init(handle, CAISS_MODE_TRAIN, dist_type_, dim_, model_path_, dist_func_);

    ret = CAISS_Train(handle, data_path_.c_str(), max_data_size_, normalize_, max_index_size_,
            precision_, fast_rank_, real_rank_, step_, max_epoch_, show_span_);

    ret = CAISS_destroyHandle(handle);
    return ret;
}

static int search() {
    /* 查询功能 */
    int ret = CAISS_RET_OK;

    void *handle = nullptr;
    ret = CAISS_CreateHandle(&handle);

    ret = CAISS_Init(handle, CAISS_MODE_PROCESS, dist_type_, dim_, model_path_, dist_func_);

    ret = CAISS_Search(handle, (void *)info_.c_str(), search_type_, top_k_);

    unsigned int size = 0;
    ret = CAISS_getResultSize(handle, size);

    char *result = new char[size + 1];
    memset(result, 0, size + 1);
    ret = CAISS_getResult(handle, result, size);
    std::cout << result << std::endl;
    delete [] result;

    ret = CAISS_destroyHandle(handle);

    return ret;
}

int main() {
    /* 使用过程中，请注意添加针对返回值ret的判定 */
    int ret = 0;
    ret = CAISS_Environment(max_thread_num_, algo_type_, manage_type_);

    ret = train();
    // ret = search();
    return 0;
}


```

## 5. 补充说明
* 训练文本样式，请看考文档中的内容

* 训练功能仅支持单线程。查询和插入功能，支持多线程并发
* 新增数据实时生效。进程重启后是否生效，取决于是否调用save方法



## 6. 版本信息

| 序号 | 变更时间 | 版本信息 | 变更人员 | 变更说明
| - | :-: | -: |  -: |  -: |  -: |  -:
| 1 | 2020.06.15 | v1.0.0 | Chunel | 新建，第一个功能版本
