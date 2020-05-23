//
// Created by Chunel on 2020/5/23.
//

#ifndef CHUNELANN_UTILSINCLUDE_H
#define CHUNELANN_UTILSINCLUDE_H

#include "../chunelAnnLib/ChunelAnnLibDefine.h"

/* 以下是会utilsCtrl的Proc.h文件总和 */
#include "./rapidJsonUtils/rapidJsonProc/RapidJsonProc.h"


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
    if (ANN_RET_OK != ret)    return ret; \

#define ANN_FUNCTION_END    \
    return ANN_RET_OK;    \

#define ANN_CHECK_MODE_ENABLE(mode, curMode)    \
    if (mode != curMode)    { return ANN_RET_MODE; }    \


#define ANN_RETURN_IF_NOT_SUCESS(ret)    \
    if (ANN_RET_OK != ret)    {return ret;}    \

#define ANN_ASSERT_NUM1_GREATER_THAN_NUM2(num1, num2)    \
    return (num1 > num2) ? (ANN_TRUE) : (ANN_FALSE);   \

#define ANN_ASSERT_NUM1_NO_LESS_THAN_NUM2(num1, num2)    \
    return (num1 >= num2) ? (ANN_TRUE) : (ANN_FALSE);   \

#define ANN_RETURN_ERR_WHEN_FALSE(input)    \
    if (input == ANN_FALSE)    { return ANN_RET_ERR }    \


#endif    //CHUNELANN_UTILSINCLUDE_H
