//
// Created by Chunel on 2020/6/3.
//

#ifndef CAISS_UTILSDEFINE_H
#define CAISS_UTILSDEFINE_H

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <cstdarg>

#define CAISS_VERSION       ("2.4.0")    // 版本信息外部不可改变


struct CaissDataNode {
    /* 读取待训练的文件的时候，表示每行的信息的数据结构 */
    std::string index;
    std::vector<CAISS_FLOAT> node;
};

struct CaissResultDetail {
    std::vector<CAISS_FLOAT> node{};
    CAISS_FLOAT distance{0.0f};    // 距离信息 （例：0.928）
    unsigned int index{0};    // 个数信息 （例：5）
    std::string label;    // 标签信息 （例：hello）

    CaissResultDetail() = default;    // 默认空值构造函数

    CaissResultDetail(const CaissResultDetail& detail) {
        this->distance = detail.distance;
        this->index = detail.index;
        this->label = detail.label;
        this->node = detail.node;    // 需要直接赋值，float的=，默认就是深拷贝
    }

    CaissResultDetail& operator=(const CaissResultDetail& detail) {
        if (this == &detail) {
            return *this;
        }

        this->distance = detail.distance;
        this->index = detail.index;
        this->label = detail.label;
        this->node = detail.node;
        return *this;
    }
};

/* 算法返回的结果类型 */
using ALOG_RET_TYPE = std::priority_queue<std::pair<CAISS_FLOAT, size_t>>;
/* 单词和算法查询的result的匹配 */
using ALOG_WORD2RESULT_MAP = std::unordered_map<std::string, ALOG_RET_TYPE>;
/* 单词和向量的匹配 */
using ALOG_WORD2VEC_MAP = std::unordered_map<std::string, std::vector<CAISS_FLOAT>>;
/* 单词和向量的和最终展示结果单匹配 */
using ALOG_WORD2DETAILS_MAP = std::unordered_map<std::string, std::list<CaissResultDetail>>;


inline void CAISS_ECHO(const char *cmd, ...) {
    std::cout << "[caiss] ";
    va_list args;
    va_start(args, cmd);
    vprintf(cmd, args);
    va_end(args);
    std::cout << "" << std::endl;
}

// 定义几个常用的宏函数
#define CAISS_DELETE_PTR(ptr) \
    if ((ptr) != nullptr)    \
    {    \
        delete (ptr);    \
        (ptr) = nullptr;    \
    }    \

#define CAISS_ASSERT_NOT_NULL(ptr)    \
    if (nullptr == (ptr))    \
    {    \
        return CAISS_RET_RES;    \
    }    \


#define CAISS_FUNCTION_BEGIN    \
    CAISS_STATUS ret = CAISS_RET_OK;    \

#define CAISS_ASSERT_ENVIRONMENT_INIT    \
    if (CAISS_TRUE != g_init) {    \
        return CAISS_RET_ERR;    \
    }    \

#define CAISS_FUNCTION_SHOW_RESULT    \
    std::cout << __FILE__ << " | " << __LINE__ << " | " << __FUNCTION__ << ", ret : " << ret << std::endl;    \

#define CAISS_FUNCTION_CHECK_STATUS    \
    if (CAISS_RET_OK != ret)   \
    {    \
        return ret;  \
    }    \

#define CAISS_FUNCTION_NO_SUPPORT    \
    return CAISS_RET_NO_SUPPORT;    \

#define CAISS_FUNCTION_END    \
    return CAISS_RET_OK;    \

#define CAISS_CHECK_MODE_ENABLE(mode)    \
    if ((mode) != this->cur_mode_)    { return CAISS_RET_MODE; }    \


#define CAISS_RETURN_IF_NOT_SUCCESS(ret)    \
    if (CAISS_RET_OK != ret)    {return ret;}    \


#endif //CAISS_UTILSDEFINE_H
