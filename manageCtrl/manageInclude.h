//
// Created by Chunel on 2020/5/24.
//

#ifndef CHUNELCAISS_ANNMANAGEINCLUDE_H
#define CHUNELCAISS_ANNMANAGEINCLUDE_H

#include "manageProc.h"
#include "./syncManage/SyncManageProc.h"
#include "./asyncManage/AsyncManageProc.h"


// todo 这里可以考虑一下，今后统一用这种方式定义function信息
using FuncCreateHandle = std::function<CAISS_RET_TYPE(void **handle)>;
using FuncDestroyHandle = std::function<CAISS_RET_TYPE(void *handle)>;
using FuncInit = std::function<CAISS_RET_TYPE(void *handle, CAISS_MODE mode, CAISS_DISTANCE_TYPE distanceType,
                                              unsigned int dim, const char *modelPath, CAISS_DIST_FUNC distFunc)>;
using FuncTrain = std::function<CAISS_RET_TYPE(void *handle, const char *dataPath, unsigned int maxDataSize, CAISS_BOOL normalize,
                                               unsigned int maxIndexSize, float precision, unsigned int fastRank,
                                               unsigned int realRank, unsigned int step, unsigned int maxEpoch,
                                               unsigned int showSpan)>;
using FuncSearch = std::function<CAISS_RET_TYPE>(void *handle, void *info, CAISS_SEARCH_TYPE searchType, unsigned int topK);
using FuncGetResultSize = std::function<CAISS_RET_TYPE>(void *handle, unsigned int &size);
using FuncGetResult = std::function<CAISS_RET_TYPE>(void *handle, char *result, unsigned int size);
using FuncInsert = std::function<CAISS_RET_TYPE>(void *handle, CAISS_FLOAT *node, const char *label, CAISS_INSERT_TYPE insertType);
using FuncSave = std::function<CAISS_RET_TYPE>(void *handle, const char *modelPath);


#endif //CHUNELCAISS_ANNMANAGEINCLUDE_H
