//
// Created by Chunel on 2020/5/23.
//

#ifndef _CHUNEL_ANN_DEFINE_H_
#define _CHUNEL_ANN_DEFINE_H_

#include <vector>

using ANN_RET_TYPE = int;
using ANN_UINT = unsigned int;
using ANN_FLOAT = float;

using ANN_VECTOR_FLOAT = std::vector<ANN_FLOAT>;
using ANN_VECTOR_UINT = std::vector<ANN_UINT>;

/* 函数返回值定义 */
#define CNL_ANN_OK     (0)
#define CNL_ANN_ERR    (-1)
#define CNL_ANN_RES    (-2)


#endif //_CHUNEL_ANN_DEFINE_H_
