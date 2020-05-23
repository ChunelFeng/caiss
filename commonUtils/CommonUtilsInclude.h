//
// Created by Chunel on 2020/5/23.
//

#ifndef CHUNELANN_COMMONUTILSINCLUDE_H
#define CHUNELANN_COMMONUTILSINCLUDE_H

#include "../chunelAnnLib/ChunelAnnLibDefine.h"

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


#define ANN_RETURN_IF_NOT_SUCESS(ret)    \
    if (ANN_RET_OK != ret)    {return ret;}    \

#define ANN_ASSERT_NUM1_GREATER_THAN_NUM2(num1, num2)    \
    return (num1 > num2) ? (ANN_TRUE) : (ANN_FALSE);   \

#define ANN_ASSERT_NUM1_NO_LESS_THAN_NUM2(num1, num2)    \
    return (num1 >= num2) ? (ANN_TRUE) : (ANN_FALSE);   \

#define ANN_RETURN_ERR_WHEN_FALSE(input)    \
    if (input == ANN_FALSE)    { return ANN_RET_ERR}    \


#endif    //CHUNELANN_COMMONUTILSINCLUDE_H
