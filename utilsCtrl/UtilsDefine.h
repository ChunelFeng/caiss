//
// Created by Chunel on 2020/6/3.
//

#ifndef CHUNELCAISS_UTILSDEFINE_H
#define CHUNELCAISS_UTILSDEFINE_H

#include <string>
#include <vector>

struct CaissDataNode {
    /* 读取待训练的文件的时候，表示每行的信息的数据结构 */
    std::string index;
    std::vector<CAISS_FLOAT> node;
};

struct CaissResultDetail {
    std::vector<CAISS_FLOAT> node;
    CAISS_FLOAT distance;    // 距离信息 （例：0.928）
    unsigned int index;    // 个数信息 （例：5）
    std::string label;    // 标签信息 （例：hello）
};

// 定义几个常用的宏函数
#define CAISS_DELETE_PTR(ptr) \
    if (ptr != nullptr)    \
    {    \
        delete ptr;    \
        ptr = nullptr;    \
    }    \

#define CAISS_ASSERT_NOT_NULL(ptr)    \
    if (nullptr == ptr)    \
    {    \
        return CAISS_RET_RES;    \
    }    \

#define CAISS_INIT_NULLPTR(ptr)    \
    ptr = nullptr;    \

#define CAISS_FUNCTION_BEGIN    \
    CAISS_RET_TYPE ret = CAISS_RET_OK;    \

#define CAISS_ASSERT_ENVIRONMENT_INIT    \
    if (CAISS_TRUE != g_init) {    \
        return CAISS_RET_ERR;    \
    }    \

#define CAISS_FUNCTION_CHECK_STATUS    \
    if (CAISS_RET_OK != ret)   \
    {    \
        return ret;  \
    }    \

#define CAISS_FUNCTION_NO_SUPPORT    \
    return CAISS_RET_NO_SUPPORT;    \

#define CAISS_FUCNTION_SHOW_RESULT    \
    std::cout << __FILE__ << " | " << __LINE__ << " | " << __FUNCTION__ << ", ret : " << ret << std::endl;    \

#define CAISS_FUNCTION_END    \
    return CAISS_RET_OK;    \

#define CAISS_CHECK_MODE_ENABLE(mode)    \
    if (mode != this->cur_mode_)    { return CAISS_RET_MODE; }    \


#define CAISS_RETURN_IF_NOT_SUCESS(ret)    \
    if (CAISS_RET_OK != ret)    {return ret;}    \

#define CAISS_ASSERT_NUM1_GREATER_THAN_NUM2(num1, num2)    \
    return (num1 > num2) ? (CAISS_TRUE) : (CAISS_FALSE);   \

#define CAISS_ASSERT_NUM1_NO_LESS_THAN_NUM2(num1, num2)    \
    return (num1 >= num2) ? (CAISS_TRUE) : (CAISS_FALSE);   \


#endif //CHUNELCAISS_UTILSDEFINE_H
