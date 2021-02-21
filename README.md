<p align="left">
  <a href="https://github.com/ChunelFeng/caiss"><img src="https://badgen.net/badge/langs/C++,Python,Java,CSharp/cyan?list=1" alt="languages"></a>
  <a href="https://github.com/ChunelFeng/caiss"><img src="https://badgen.net/badge/os/MacOS,Linux,Windows/cyan?list=1" alt="os"></a>
  <a href="https://github.com/ChunelFeng/caiss"><img src="https://badgen.net/badge/support/sdk,sql,restful/cyan?list=1" alt="support"></a><br>
  <a href="https://github.com/ChunelFeng/caiss/stargazers"><img src="https://badgen.net/github/stars/ChunelFeng/caiss?color=cyan" alt="stars"></a>
  <a href="https://github.com/ChunelFeng/caiss/network/members"><img src="https://badgen.net/github/forks/ChunelFeng/caiss?color=cyan" alt="forks"></a>
  <a href="http://www.chunel.cn"><img src="https://badgen.net/badge/🔥/forum/cyan" alt="forum"></a>
  <a href="https://hub.docker.com/r/chunelfeng/caiss"><img src="https://badgen.net/badge/🐳/docker/cyan?list=1" alt="docker"></a>
  <a href="http://www.chunel.cn:3001/"><img src="https://badgen.net/badge/👀/web/cyan" alt="web"></a>
  <a href="https://github.com/ChunelFeng/caiss"><img src="https://badgen.net/badge/🌟/github/cyan" alt="github"></a>
  </a>
</p>

<h1 align="center">
  Caiss说明文档
</h1>

## 一. 简介

&emsp;&emsp;随着人工智能技术的普及，海量高维度向量的相似度查询技术在研究和生产中的作用和重要性与日俱增。目前，市面有许多优秀开源的解决方案，但是，在使用过程中遇到了一些问题，比如：
* 由于对于各种算法原理的了解不深，不会调整参数，导致的训练模型结果偏差较大。
* 开源库对于各种距离的支持有限，无法满足随时变化的实验需求。
* 标签信息和向量的分离，导致标记和查询需要在不同的步骤中完成。
* 功能不够全面，无法覆盖日常需要的"增删改查"功能。
* 部分解决方案，对于平台或者对于编程语言的依赖，导致了各种环境问题。

&emsp;&emsp;在这里，我们基于Google，Facebook，Alibaba等科技巨头的现有成果，实现了一套全新思路的海量数据检索开源解决方案。提供面向最终结果的训练方法，在训练过程中根据设定的目标自动调节参数。提供常用距离和自定义距离的训练和查询方式。支持缓存和多线程调用，支持批量查询功能。提供纯C风格的SDK接口，提供多种语言（如：Python，Java，C#）的版本，支持基础SQL语法进行增删查改，支持MacOS，Linux和Windows系统。提供了详细的Demo示例，并提供了Docker镜像和Web页面展示。

&emsp;&emsp;我们把这套解决方案，命名为Caiss(Chunel Artificial Intelligence Similarity Search)。经过实测，它可以在保持超高准确率的情况下，极大的降低查询耗时。且随着数据量的增加，其性能上的优势会更加明显。希望它可以在大家的研究和生产过程中，发挥积极的作用。

&emsp;&emsp;&emsp;&emsp;&emsp;&emsp; ![Caiss Logo](https://github.com/ChunelFeng/caiss/blob/master/doc/image/Caiss%20Logo.jpg)


## 二. 编译说明

* Caiss在Mac(MacOS-10.15，主环境)，Linux（Ubuntu-20.04）和Windows（Windows-10）上开发，使用的IDE均是CLion。使用Clion读取CMakeList.txt文件，并生成对应工程，将CMakeList.txt文件中【MY_BOOST_PATH】和【MY_EIGEN_PATH】修改为新环境中boost库和eigen库的位置，即可完成编译。
* Linux环境开发者（以C++为例），命令行模式下，输入:
  ```shell
  $ git clone https://github.com.cnpmjs.org/ChunelFeng/caiss.git    # 下载代码至本地
  $ cd caiss/
  $ ./caiss-linux-env.sh                     # 一键安装Caiss在Linux（Ubuntu）上的运行环境    
  $ cmake .                                  # 注意，cmake后面有一个"."，表示当前目录    
  $ make                                     # 即可完成编译    
  $ cd caissDemo/    
  $ ./CaissDemo                              # 即可查看C++版本demo的运行结果    
  ```
* Docker环境开发者（以Python为例），输入:
  ```shell
  $ docker pull chunelfeng/caiss                        # 获取Caiss的最新Docker版本
  $ docker run -it --name CaissDemo chunelfeng/caiss    # 启动容器，并进入内部环境
  $ cmake .                                             # 注意，cmake后面有一个"."，表示当前目录
  $ make 
  $ cd python/    
  $ python3 pyCaissDemo.py                              # 即可查看python版本demo的运行结果
  ```
* Windows上，开发环境为Visual Studio的C++开发者，请使用[feature/for-windows-visual-studio]分支，通过CMakeList.txt文件自动生成对应的*.sln文件，然后通过Visual Studio打开，即可完成编译。
* Web端或者移动端，可以点击进入 [Caiss 网页版](http://www.chunel.cn:3001) 查看简易展示效果。网页版暂时仅支持英文单词的相似语义词语查询，多词查询请使用"|"分隔，例：hello|world
* 如果在编译或使用过程中遇到任何问题，欢迎随时联系我们（联系方式见附录）。我们很乐意跟您一起探讨和解决使用过程中可能遇到的任何问题，并携手做进一步优化。

![Caiss 网页版界面](https://github.com/ChunelFeng/caiss/blob/master/doc/image/Caiss%20Web.png)


## 三. 训练流程

1，安装python3环境，安装tensorflow库，安装keras库，安装keras-bert库，安装numpy库，安装pprint库。

2，根据需求，下载对应的bert模型并解压至本地。bert模型下载，请参考链接：[bert入门资料和模型下载地址](http://chunel.cn/archives/knowledge-of-bert)。

3，准备待embedding的文本文件。比如，英文单词的相似词查询任务，将不同的单词按行分开即可。具体格式，如下所示：
  ```shell script
  one
  two
  three
  four
  ```

4，执行/python/dataProcess/pyCaissTrainDataBuilder.py中下的__main__方法。执行前，需要根据实际情况，修改待embedding文本的位置(embedding_file_path)，bert模型的位置(bert_model_path)。函数执行完毕后，会在result_path位置，生成可用于Caiss训练的文本内容。具体格式，如下所示：
  ```shell script
  {"one": ["1.0", "0.0", "0.0", "0.0"]}
  {"two": ["0.0", "1.0", "0.0", "0.0"]}
  {"three": ["0.0", "0.0", "1.0", "0.0"]}
  {"four": ["0.0", "0.0", "0.0", "1.0"]}
  ```

5，参考下文第5部分关于Caiss的使用demo，开始训练、查询等功能吧。

![Caiss 架构设计图](https://github.com/ChunelFeng/caiss/blob/master/doc/image/Caiss%20Skeleton.png)

## 四. 接口定义
```cpp
/**
 * 初始化环境信息
 * @param maxThreadSize 支持的最大并发数
 * @param algoType 算法类型（详见CaissLibDefine.h文件）
 * @param manageType 并发类型（详见CaissLibDefine.h文件）
 * @return 运行成功返回0，警告返回1，其他异常值，参考错误码定义
 */
CAISS_API CAISS_Environment(CAISS_UINT maxThreadSize = 1,
                            CAISS_ALGO_TYPE algoType = CAISS_ALGO_DEFAULT,
                            CAISS_MANAGE_TYPE manageType = CAISS_MANAGE_DEFAULT);

/**
 * 创建句柄信息
 * @param handle 句柄信息
 * @return 运行成功返回0，警告返回1，其他异常值，参考错误码定义
 */
CAISS_API CAISS_CreateHandle(CAISS_HANDLE *handle);

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
CAISS_API CAISS_Init(CAISS_HANDLE handle,
                     CAISS_MODE mode,
                     CAISS_DISTANCE_TYPE distanceType,
                     CAISS_UINT dim,
                     CAISS_STRING modelPath,
                     CAISS_DIST_FUNC distFunc = nullptr);

/**
 * 模型训练功能
 * @param handle 句柄信息
 * @param dataPath 待训练样本路径（训练文件格式，参考/doc/文件夹下demo_2500words_768dim.txt的格式）
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
 * @notice 当快速查询fastRank个数，均在真实realRank个数的范围内的准确率，超过precision的时候，训练完成
 */
CAISS_API CAISS_Train(CAISS_HANDLE handle,
                      CAISS_STRING dataPath,
                      CAISS_UINT maxDataSize,
                      CAISS_BOOL normalize,
                      CAISS_UINT maxIndexSize = 64,
                      CAISS_FLOAT precision = 0.95,
                      CAISS_UINT fastRank = 5,
                      CAISS_UINT realRank = 5,
                      CAISS_UINT step = 1,
                      CAISS_UINT maxEpoch = 5,
                      CAISS_UINT showSpan = 1000);

/**
 * 查询功能
 * @param handle 句柄信息
 * @param info 待查询的信息
 * @param searchType 查询信息的类型（详见CaissLibDefine.h文件）
 * @param topK 返回最近的topK个信息
 * @param filterEditDistance 需要过滤的最小词语编辑距离
 * @param searchCBFunc 查询到结果后，执行回调函数，传入的是查询到结果的word信息和distance信息
 * @param cbParams 回调函数中，传入的参数信息
 * @return 运行成功返回0，警告返回1，其他异常值，参考错误码定义
 * @notice filterEditDistance仅针对根据单词过滤的情况下生效。
 *         =-1表示不过滤；=0表示过滤跟当前词语完全相同的；
 *         =3表示过滤跟当前词语相编辑距离的在3以内的，以此类推；
 *         最大值不超过CAISS_MAX_EDIT_DISTANCE值
 */
CAISS_API CAISS_Search(CAISS_HANDLE handle,
                       void *info,
                       CAISS_SEARCH_TYPE searchType,
                       CAISS_UINT topK,
                       CAISS_UINT filterEditDistance = CAISS_DEFAULT_EDIT_DISTANCE,
                       CAISS_SEARCH_CALLBACK searchCBFunc = nullptr,
                       const void *cbParams = nullptr);

/**
 * 获取结果字符串长度
 * @param handle 句柄信息
 * @param size 结果长度
 * @return 运行成功返回0，警告返回1，其他异常值，参考错误码定义
 */
CAISS_API CAISS_GetResultSize(CAISS_HANDLE handle,
                              CAISS_UINT &size);

/**
 * 获取查询结果信息
 * @param handle 句柄信息
 * @param result 结果信息
 * @param size 对应结果长度
 * @return 运行成功返回0，警告返回1，其他异常值，参考错误码定义
 */
CAISS_API CAISS_GetResult(CAISS_HANDLE handle,
                          char *result,
                          CAISS_UINT size);

/**
 * 插入信息
 * @param handle 句柄信息
 * @param node 待插入的向量信息
 * @param label 待插入向量的标签信息
 * @param insertType 插入类型（详见CaissLibDefine.h文件）
 * @return 运行成功返回0，警告返回1，其他异常值，参考错误码定义
 * @notice 插入信息实时生效。程序结束后，是否保存新插入的信息，取决于是否调用CAISS_Save()方法
 */
CAISS_API CAISS_Insert(CAISS_HANDLE handle,
                       CAISS_FLOAT *node,
                       CAISS_STRING label,
                       CAISS_INSERT_TYPE insertType);

/**
 * 忽略信息
 * @param handle 句柄信息
 * @param label 待忽略的标签信息
 * @param isIgnore 表示忽略（true）或者不再忽略（false）
 * @return 运行成功返回0，警告返回1，其他异常值，参考错误码定义
 */
CAISS_API CAISS_Ignore(CAISS_HANDLE handle,
                       CAISS_STRING label,
                       CAISS_BOOL isIgnore = CAISS_TRUE);

/**
 * 保存模型
 * @param handle 句柄信息
 * @param modelPath 模型保存路径（默认值是覆盖当前模型）
 * @return 运行成功返回0，警告返回1，其他异常值，参考错误码定义
 */
CAISS_API CAISS_Save(CAISS_HANDLE handle,
                     CAISS_STRING modelPath = nullptr);

/**
 * 执行sql指令
 * @param handle 句柄信息
 * @param sql 查询的sql语句
 * @param sqlCBFunc 执行sql过程中，触发的回调函数
 * @param sqlParams 传入的条件信息
 * @return 运行成功返回0，警告返回1，其他异常值，参考错误码定义
 */
CAISS_API CAISS_ExecuteSQL(CAISS_HANDLE handle,
                           CAISS_STRING sql,
                           CAISS_SEARCH_CALLBACK sqlCBFunc = nullptr,
                           const void *sqlParams = nullptr);

/**
 * 销毁句柄信息
 * @param handle 句柄信息
 * @return 运行成功返回0，警告返回1，其他异常值，参考错误码定义
 */
CAISS_API CAISS_DestroyHandle(CAISS_HANDLE handle);
```

## 五. 使用Demo
```cpp
/*
* 更多使用样例，请参考caissDemo文件夹中内容。
* 使用过程中，请对每个CAISS_*函数的返回值进行判断和处理。
* doc文件夹中，提供了demo_2500words_768dim.txt文件。
* 如果需要训练新的Caiss模型，请根据此文件的样式，生成新的词向量文件。
*/

#include <iostream>
#include "CaissLib.h"


static const CAISS_UINT max_thread_num_ = 1;    // 线程数量
static const CAISS_ALGO_TYPE algo_type_ = CAISS_ALGO_HNSW;
static const CAISS_MANAGE_TYPE manage_type_ = CAISS_MANAGE_SYNC;
static const CAISS_MODE mode_ = CAISS_MODE_PROCESS;
static const CAISS_DISTANCE_TYPE dist_type_ = CAISS_DISTANCE_INNER;
static const CAISS_UINT dim_ = 768;    // 向量维度
static const CAISS_STRING model_path_ = "./demo_2500words_768dim.caiss";    // caiss模型路径
static const CAISS_DIST_FUNC dist_func_ = nullptr;
static const CAISS_STRING info_ = "water";    // 多词查询的情况下，输入使用"|"分隔。例："hello|world"
static const CAISS_SEARCH_TYPE search_type_ = CAISS_SEARCH_WORD;
static const CAISS_UINT top_k_ = 5;

static const CAISS_STRING data_path_ = "./demo_2500words_768dim.txt";    // caiss训练文件路径
static const CAISS_UINT max_data_size_ = 5000;    // 建议略大于训练样本中的行数，方便今后插入数据的更新
static const CAISS_BOOL normalize_ = CAISS_TRUE;    // 是否对数据进行归一化处理（常用于计算cos距离）
static const CAISS_UINT max_index_size_ = 64;     // 标签的最大长度
static const CAISS_FLOAT precision_ = 0.98f;    // 模型精确度
static const CAISS_UINT fast_rank_ = 5;
static const CAISS_UINT real_rank_ = 5;
static const CAISS_UINT step_ = 1;
static const CAISS_UINT max_epoch_ = 3;
static const CAISS_UINT show_span_ = 1000;

static CAISS_STATUS train() {
    /* 训练功能 */
    CAISS_STATUS ret = CAISS_RET_OK;

    CAISS_HANDLE handle = nullptr;
    ret = CAISS_CreateHandle(&handle);

    ret = CAISS_Init(handle, CAISS_MODE_TRAIN, dist_type_, dim_, model_path_, dist_func_);

    ret = CAISS_Train(handle, data_path_, max_data_size_, normalize_, max_index_size_,
            precision_, fast_rank_, real_rank_, step_, max_epoch_, show_span_);

    ret = CAISS_DestroyHandle(handle);
    return ret;
}

static CAISS_STATUS search() {
    /* 查询功能 */
    CAISS_STATUS ret = CAISS_RET_OK;

    CAISS_HANDLE handle = nullptr;
    ret = CAISS_CreateHandle(&handle);

    ret = CAISS_Init(handle, CAISS_MODE_PROCESS, dist_type_, dim_, model_path_, dist_func_);

    ret = CAISS_Search(handle, (void *)info_, search_type_, top_k_);

    unsigned int size = 0;
    ret = CAISS_GetResultSize(handle, size);

    char *result = new char[size + 1];
    memset(result, 0, size + 1);
    ret = CAISS_GetResult(handle, result, size);
    std::cout << result << std::endl;    // 输出结果格式，见下文
    delete [] result;

    ret = CAISS_DestroyHandle(handle);

    return ret;
}

int main() {
    /* 使用过程中，请注意添加针对返回值ret的判定 */
    CAISS_STATUS ret = CAISS_RET_OK;
    ret = CAISS_Environment(max_thread_num_, algo_type_, manage_type_);
    if (CAISS_RET_OK != ret) {
        return ret;    // 针对CAISS_*函数返回值，进行异常判定
    }

    ret = train();
    ret = search();
    return 0;
}
```


## 六. 输出内容

* 训练接口执行完毕后，会在对应的目录下生成 *.caiss 模型文件。不同操作操作系统之间生成的模型文件，不能混用。如需跨平台使用，请重新训练。
* 查询结果输出，为标准json格式。例：查询词语water，topK=5，返回相似词语为：[food,glass,light,alcohol,liquid]这5个词语。耗时信息单位为微秒。查询结果信息如下：

```json
{
    "version":"3.0.0",
    "size":5,
    "distance_type":"inner",
    "search_type":"ann_search",
    "ann_ts":2692,
    "search_ts":3721,
    "algo_type":"hnsw",
    "result":[
        {
            "query":"water",
            "details":[
                {
                    "distance":0.10039210319519043,
                    "index":641,
                    "label":"food"
                },
                {
                    "distance":0.10391676425933838,
                    "index":1812,
                    "label":"glass"
                },
                {
                    "distance":0.11682325601577759,
                    "index":361,
                    "label":"light"
                },
                {
                    "distance":0.12232851982116699,
                    "index":2163,
                    "label":"alcohol"
                },
                {
                    "distance":0.12482517957687378,
                    "index":2281,
                    "label":"liquid"
                }
            ]
        }
    ]
}
```


## 七. 其他

* 训练功能仅支持单线程。查询、插入、修改和删除等功能，支持多线程并发。
* 插入、修改或删除数据，实时生效。进程重启后是否生效，取决于是否调用save方法。
* 在异步模式下，查询、插入等需要传入向量信息的方法中，请自行保证传入的向量数据在内存中持续存在，直至结果生效为止。
* Caiss的源代码，发布在：https://github.com/ChunelFeng/caiss ，技术交流论坛地址：[一面之猿网](http://www.chunel.cn)，欢迎随时交流指导和贡献代码。如有使用需求，周末可提供支持服务。
* 感谢知名AI科技博主——北邮PRIS模式识别实验室 陈光教授 [@爱可可-爱生活](https://weibo.com/fly51fly?profile_ftype=1&is_hot=1#_0) 微博推荐。

![感谢教授推荐](https://github.com/ChunelFeng/caiss/blob/master/doc/image/Caiss%20Recommend.jpg)

------------
#### 附录-1. 版本信息

[2020.06.15 - v1.0.0 - Chunel] 
* 新建，第一个功能版本
* 提供训练、查询、插入、保存等功能

[2020.06.25 - v1.2.0 - Chunel]
* 新增多线程功能
* 新增缓存机制
* 提供简单的使用demo
* 兼容mac和Linux系统

[2020.06.29 - v1.2.1 - Chunel]
* 异步并发模式优化

[2020.07.03 - v1.3.0 - Chunel]
* 新增根据编辑距离的过滤
* 新增python版本，提供基础查询功能

[2020.07.11 - v1.5.0 - Chunel]
* 新增词语删除功能
* 新增并行计算功能，极大缩短训练和验证耗时
* 基于python版本，对外提供网络服务

[2020.07.18 - v1.5.1 - Chunel]
* 优化异步查询过程中，查询单词信息内存自动释放的问题

[2020.08.01 - v1.5.2 - Chunel]
* 提供并行计算方法，进一步减少查询耗时
* 解决跨平台兼容性问题

[2020.08.22 - v1.6.0 - Chunel]
* 提供降维算法
* 提供针对文本的embedding处理方法，用于自行生成符合格式的训练文件

[2020.08.30 - v2.0.0 - Chunel]
* 提供基础SQL语句查询功能

[2020.09.12 - v2.0.1 - Chunel]
* 提供基于SQL语句的增删查改功能
* 提供SQL版本的使用demo

[2020.09.19 - v2.1.0 - Chunel]
* 提供Java版本的SDK接口及其对应的使用demo

[2020.10.01 - v2.2.0 - Chunel]
* 优化文本训练方式，解决了针对tensorflow-v2.0及其以上版本的兼容性问题
* 提供C#版本的SDK接口及其对应的使用demo

[2020.10.31 - v2.4.0 - Chunel]
* 提供批量查询功能
* 优化内部缓存结构
* 修改输出json格式

[2020.11.15 - v2.4.1 - Chunel]
* 提供Linux中一键安装加速环境功能
* 优化Python版本及其对应的使用Demo
* 修复已知问题，异常流程添加主动报错信息，优化使用体验

[2020.11.21 - v2.5.0 - Chunel]
* 提供Docker版本
* 提供静默运行功能，优化提示信息

[2020.12.13 - v3.0.0 - Chunel]
* 提供基于MRPT算法的训练和查询功能
* 更新输出内容，加入算法类型和耗时信息

[2021.02.09 - v3.0.1 - [Awind](https://github.com/awind)]
* 提供Web页面版本

------------
#### 附录-2. 推荐阅读: 
* [Why Caiss —— 谈谈我为什么要做Caiss](http://www.chunel.cn/archives/whycaiss)
* [What Caiss（上）—— 谈谈我在Caiss中，做了哪些算法优化](http://www.chunel.cn/archives/whatcaiss)
* [What Caiss（中）—— 谈谈我在Caiss中，做了哪些工程优化](http://www.chunel.cn/archives/whatcaiss2)
* [What Caiss（下）—— 谈谈我在Caiss中，做了哪些服务化工作](http://www.chunel.cn/archives/whatcaiss3)
* [How Caiss —— 谈谈我实测的Caiss的性能指标如何](http://www.chunel.cn/archives/howcaiss)
* [Where Caiss —— 谈谈我关于Caiss下一步做什么的想法](http://www.chunel.cn/archives/wherecaiss)

------------
#### 附录-3. 联系方式: 

* 微信： ChunelFeng
* 邮箱： chunel@foxmail.com
* 源码： https://github.com/ChunelFeng/caiss
* 论坛： www.chunel.cn

![Caiss 作者微信二维码](https://github.com/ChunelFeng/caiss/blob/master/doc/image/Caiss%20Author.jpg)

