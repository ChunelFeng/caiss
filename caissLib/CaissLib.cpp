#include "CaissLib.h"
#include "../manageCtrl/ManageInclude.h"

static ManageProc* g_manage = nullptr;
static CAISS_BOOL g_init = CAISS_FALSE;
static RWLock g_lock;

static ManageProc* createManage(const unsigned int maxThreadSize,
                                const CAISS_ALGO_TYPE algoType,
                                const CAISS_MANAGE_TYPE mangeType) {
    ManageProc* manage = nullptr;
    switch (mangeType) {
        case CAISS_MANAGE_SYNC:
            manage = new SyncManageProc(maxThreadSize, algoType);
            break;
        case CAISS_MANAGE_ASYNC:
            manage = new AsyncManageProc(maxThreadSize, algoType);
            break;
        default:
            break;
    }

    return manage;
}

CAISS_LIB_API CAISS_RET_TYPE STDCALL CAISS_Environment(const unsigned int maxThreadSize,
                                                       const CAISS_ALGO_TYPE algoType,
                                                       const CAISS_MANAGE_TYPE manageType) {
    CAISS_FUNCTION_BEGIN
    if (nullptr == g_manage) {
        g_lock.writeLock();
        if (nullptr == g_manage) {
            g_manage = createManage(maxThreadSize, algoType, manageType);
            g_init = CAISS_TRUE;    // 通过init参数，来确定环境是否初始化。如果初始化了，则不需要进行
        }
        g_lock.writeUnlock();
    } else {
        ret = CAISS_RET_WARNING;    // 多次初始化，给出警告信息
    }

    CAISS_FUNCTION_CHECK_STATUS
    CAISS_FUNCTION_END
}


CAISS_LIB_API CAISS_RET_TYPE STDCALL CAISS_CreateHandle(void** handle) {
    CAISS_ASSERT_ENVIRONMENT_INIT
    return g_manage->createHandle(handle);
}


CAISS_LIB_API CAISS_RET_TYPE STDCALL CAISS_Init(void* handle,
                                                const CAISS_MODE mode,
                                                const CAISS_DISTANCE_TYPE distanceType,
                                                const unsigned int dim,
                                                const char *modelPath,
                                                const CAISS_DIST_FUNC distFunc) {
    CAISS_ASSERT_ENVIRONMENT_INIT
    return g_manage->init(handle, mode, distanceType, dim, modelPath, distFunc);
}


CAISS_LIB_API CAISS_RET_TYPE STDCALL CAISS_Train(void *handle,
                                                 const char *dataPath,
                                                 const unsigned int maxDataSize,
                                                 const CAISS_BOOL normalize,
                                                 const unsigned int maxIndexSize,
                                                 const float precision,
                                                 const unsigned int fastRank,
                                                 const unsigned int realRank,
                                                 const unsigned int step,
                                                 const unsigned int maxEpoch,
                                                 const unsigned int showSpan) {
    CAISS_ASSERT_ENVIRONMENT_INIT;
    return g_manage->train(handle, dataPath, maxDataSize, normalize, maxIndexSize, precision, fastRank, realRank, step, maxEpoch, showSpan);
}

CAISS_LIB_API CAISS_RET_TYPE STDCALL CAISS_Search(void *handle,
                                                  void *info,
                                                  const CAISS_SEARCH_TYPE searchType,
                                                  const unsigned int topK,
                                                  const unsigned int filterEditDistance,
                                                  const CAISS_SEARCH_CALLBACK searchCBFunc,
                                                  const void *cbParams) {
    CAISS_ASSERT_ENVIRONMENT_INIT
    return g_manage->search(handle, info, searchType, topK, filterEditDistance, searchCBFunc, cbParams);
}


CAISS_LIB_API CAISS_RET_TYPE STDCALL CAISS_GetResultSize(void *handle,
                                                         unsigned int &size) {
    CAISS_ASSERT_ENVIRONMENT_INIT
    return g_manage->getResultSize(handle, size);
}


CAISS_LIB_API CAISS_RET_TYPE STDCALL CAISS_GetResult(void *handle,
                                                     char *result,
                                                     const unsigned int size) {
    CAISS_ASSERT_ENVIRONMENT_INIT
    return g_manage->getResult(handle, result, size);
}


CAISS_LIB_API CAISS_RET_TYPE STDCALL CAISS_Insert(void *handle,
                                                  CAISS_FLOAT *node,
                                                  const char *label,
                                                  CAISS_INSERT_TYPE insertType) {
    CAISS_ASSERT_ENVIRONMENT_INIT
    return g_manage->insert(handle, node, label, insertType);
}


CAISS_LIB_API CAISS_RET_TYPE STDCALL CAISS_Ignore(void *handle,
                                                  const char *label,
                                                  const CAISS_BOOL isIgnore) {
    CAISS_ASSERT_ENVIRONMENT_INIT
    return g_manage->ignore(handle, label, isIgnore);
}


CAISS_LIB_API CAISS_RET_TYPE STDCALL CAISS_Save(void *handle,
                                                const char *modelPath) {
    CAISS_ASSERT_ENVIRONMENT_INIT
    return g_manage->save(handle, modelPath);
}

CAISS_LIB_API CAISS_RET_TYPE STDCALL CAISS_ExecuteSQL(void *handle,
                                                      const char *sql,
                                                      CAISS_SQL_CALLBACK sqlCBFunc,
                                                      const void *sqlParams) {
    CAISS_ASSERT_ENVIRONMENT_INIT
    return g_manage->executeSQL(handle, sql, sqlCBFunc, sqlParams);
}


CAISS_LIB_API CAISS_RET_TYPE STDCALL CAISS_DestroyHandle(void *handle) {
    CAISS_ASSERT_ENVIRONMENT_INIT
    return g_manage->destroyHandle(handle);
}
