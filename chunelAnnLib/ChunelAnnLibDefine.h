//
// Created by Chunel on 2020/5/23.
//

#ifndef CHUNELANN_CHUNELANNLIBDEFINE_H
#define CHUNELANN_CHUNELANNLIBDEFINE_H

#include <vector>

using ANN_RET_TYPE = int;
using ANN_UINT = unsigned int;
using ANN_FLOAT = float;

using ANN_VECTOR_FLOAT = std::vector<ANN_FLOAT>;
using ANN_VECTOR_UINT = std::vector<ANN_UINT>;

/* 函数返回值定义 */
#define IFT_ANN_OK     (0)
#define IFT_ANN_ERR    (-1)
#define IFT_ANN_RES    (-2)


#endif //CHUNELANN_CHUNELANNLIBDEFINE_H
