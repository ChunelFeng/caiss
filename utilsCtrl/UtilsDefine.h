//
// Created by Chunel on 2020/6/3.
//

#ifndef CHUNELANN_UTILSDEFINE_H
#define CHUNELANN_UTILSDEFINE_H

#include <string>
#include <vector>

struct AnnDataNode {
    /* 读取待训练的文件的时候，表示每行的信息的数据结构 */
    std::string index;
    std::vector<ANN_FLOAT> node;
};

struct AnnResultDetail {
    std::vector<ANN_FLOAT> node;
    ANN_FLOAT distance;    // 距离信息 （例：0.928）
    unsigned int index;    // 个数信息 （例：1）
    std::string label;    // 标签信息 （例：hello）
};

// 定义几个常用的宏函数
#define ANN_DELETE_PTR(ptr) \
    if (ptr != nullptr)    \
    {    \
        delete ptr;    \
        ptr = nullptr;    \
    }    \

#define ANN_ASSERT_NOT_NULL(ptr)    \
    if (nullptr == ptr)    \
    {    \
        return ANN_RET_RES;    \
    }    \

#define ANN_INIT_NULLPTR(ptr)    \
    ptr = nullptr;    \

#define ANN_FUNCTION_BEGIN    \
    ANN_RET_TYPE ret = ANN_RET_OK;    \

#define ANN_FUNCTION_CHECK_STATUS    \
    std::cout << __FILE__ << " | " << __LINE__ << " | " << __FUNCTION__ << ", ret : " << ret << std::endl;    \
    if (ANN_RET_OK != ret)   \
    {    \
        std::cout << __FILE__ << " | " << __LINE__ << " | " << __FUNCTION__ << ", ret : " << ret << std::endl;    \
        return ret; \
    }    \

#define ANN_FUNCTION_END    \
    return ANN_RET_OK;    \

#define ANN_CHECK_MODE_ENABLE(mode)    \
    if (mode != this->cur_mode_)    { return ANN_RET_MODE; }    \


#define ANN_RETURN_IF_NOT_SUCESS(ret)    \
    if (ANN_RET_OK != ret)    {return ret;}    \

#define ANN_ASSERT_NUM1_GREATER_THAN_NUM2(num1, num2)    \
    return (num1 > num2) ? (ANN_TRUE) : (ANN_FALSE);   \

#define ANN_ASSERT_NUM1_NO_LESS_THAN_NUM2(num1, num2)    \
    return (num1 >= num2) ? (ANN_TRUE) : (ANN_FALSE);   \

#define ANN_RETURN_ERR_WHEN_FALSE(input)    \
    if (input == ANN_FALSE)    { return ANN_RET_ERR }    \

#endif //CHUNELANN_UTILSDEFINE_H
