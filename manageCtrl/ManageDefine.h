//
// Created by Chunel on 2020/6/20.
//

#ifndef CAISS_MANAGEDEFINE_H
#define CAISS_MANAGEDEFINE_H

#include <functional>

#include "../caissLib/CaissLibDefine.h"


using FuncCreateHandle = std::function<CAISS_STATUS(void **handle)>;
using FuncDestroyHandle = std::function<CAISS_STATUS(void *handle)>;
using FuncInit = std::function<CAISS_STATUS(void *handle, CAISS_MODE mode, CAISS_DISTANCE_TYPE distanceType,
                                            unsigned int dim, const char *modelPath, CAISS_DIST_FUNC distFunc)>;
using FuncTrain = std::function<CAISS_STATUS(void *handle, const char *dataPath, unsigned int maxDataSize, CAISS_BOOL normalize,
                                             unsigned int maxIndexSize, float precision, unsigned int fastRank,
                                             unsigned int realRank, unsigned int step, unsigned int maxEpoch,
                                             unsigned int showSpan)>;
using FuncSearch = std::function<CAISS_STATUS(void *info, CAISS_SEARCH_TYPE searchType, unsigned int topK)>;
using FuncGetResultSize = std::function<CAISS_STATUS(void *handle, unsigned int &size)>;
using FuncGetResult = std::function<CAISS_STATUS(void *handle, char *result, unsigned int size)>;
using FuncInsert = std::function<CAISS_STATUS(void *handle, CAISS_FLOAT *node, const char *label, CAISS_INSERT_TYPE insertType)>;
using FuncSave = std::function<CAISS_STATUS(void *handle, const char *modelPath)>;

#endif //CAISS_MANAGEDEFINE_H
