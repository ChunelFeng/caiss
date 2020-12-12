#include "CaissLib.h"
#include "../manageCtrl/ManageInclude.h"

static ManageProc* g_manage = nullptr;
static CAISS_BOOL g_init = CAISS_FALSE;
static RWLock g_lock;

static ManageProc* createManage(const CAISS_UINT maxThreadSize,
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


CAISS_LIB_API CAISS_STATUS STDCALL CAISS_Environment(const CAISS_UINT maxThreadSize,
                                                     const CAISS_ALGO_TYPE algoType,
                                                     const CAISS_MANAGE_TYPE manageType) {
    CAISS_FUNCTION_BEGIN
    if (algoType != CAISS_ALGO_HNSW && algoType != CAISS_ALGO_MRPT) {
        return CAISS_RET_NO_SUPPORT;    // 暂时仅支持hsnw和mrpt算法
    }

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


CAISS_LIB_API CAISS_STATUS STDCALL CAISS_CreateHandle(CAISS_HANDLE *handle) {
    CAISS_ASSERT_ENVIRONMENT_INIT
    return g_manage->createHandle(handle);
}


CAISS_LIB_API CAISS_STATUS STDCALL CAISS_Init(CAISS_HANDLE handle,
                                              const CAISS_MODE mode,
                                              const CAISS_DISTANCE_TYPE distanceType,
                                              const CAISS_UINT dim,
                                              CAISS_STRING modelPath,
                                              const CAISS_DIST_FUNC distFunc) {
    CAISS_ASSERT_ENVIRONMENT_INIT
    return g_manage->init(handle, mode, distanceType, dim, modelPath, distFunc);
}


CAISS_LIB_API CAISS_STATUS STDCALL CAISS_Train(CAISS_HANDLE handle,
                                               CAISS_STRING dataPath,
                                               const CAISS_UINT maxDataSize,
                                               const CAISS_BOOL normalize,
                                               const CAISS_UINT maxIndexSize,
                                               const CAISS_FLOAT precision,
                                               const CAISS_UINT fastRank,
                                               const CAISS_UINT realRank,
                                               const CAISS_UINT step,
                                               const CAISS_UINT maxEpoch,
                                               const CAISS_UINT showSpan) {
    CAISS_ASSERT_ENVIRONMENT_INIT;
    return g_manage->train(handle, dataPath, maxDataSize, normalize, maxIndexSize, precision, fastRank, realRank, step, maxEpoch, showSpan);
}


CAISS_LIB_API CAISS_STATUS STDCALL CAISS_Search(CAISS_HANDLE handle,
                                                void *info,
                                                const CAISS_SEARCH_TYPE searchType,
                                                const CAISS_UINT topK,
                                                const CAISS_UINT filterEditDistance,
                                                const CAISS_SEARCH_CALLBACK searchCBFunc,
                                                const void *cbParams) {
    CAISS_ASSERT_ENVIRONMENT_INIT
    return g_manage->search(handle, info, searchType, topK, filterEditDistance, searchCBFunc, cbParams);
}


CAISS_LIB_API CAISS_STATUS STDCALL CAISS_GetResultSize(CAISS_HANDLE handle,
                                                       CAISS_UINT &size) {
    CAISS_ASSERT_ENVIRONMENT_INIT
    return g_manage->getResultSize(handle, size);
}


CAISS_LIB_API CAISS_STATUS STDCALL CAISS_GetResult(CAISS_HANDLE handle,
                                                   char *result,
                                                   const CAISS_UINT size) {
    CAISS_ASSERT_ENVIRONMENT_INIT
    return g_manage->getResult(handle, result, size);
}


CAISS_LIB_API CAISS_STATUS STDCALL CAISS_Insert(CAISS_HANDLE handle,
                                                CAISS_FLOAT *node,
                                                CAISS_STRING label,
                                                CAISS_INSERT_TYPE insertType) {
    CAISS_ASSERT_ENVIRONMENT_INIT
    return g_manage->insert(handle, node, label, insertType);
}


CAISS_LIB_API CAISS_STATUS STDCALL CAISS_Ignore(CAISS_HANDLE handle,
                                                CAISS_STRING label,
                                                const CAISS_BOOL isIgnore) {
    CAISS_ASSERT_ENVIRONMENT_INIT
    return g_manage->ignore(handle, label, isIgnore);
}


CAISS_LIB_API CAISS_STATUS STDCALL CAISS_Save(CAISS_HANDLE handle,
                                              CAISS_STRING modelPath) {
    CAISS_ASSERT_ENVIRONMENT_INIT
    return g_manage->save(handle, modelPath);
}


CAISS_LIB_API CAISS_STATUS STDCALL CAISS_ExecuteSQL(CAISS_HANDLE handle,
                                                    CAISS_STRING sql,
                                                    CAISS_SEARCH_CALLBACK sqlCBFunc,
                                                    const void *sqlParams) {
    CAISS_ASSERT_ENVIRONMENT_INIT
    return g_manage->executeSQL(handle, sql, sqlCBFunc, sqlParams);
}


CAISS_LIB_API CAISS_STATUS STDCALL CAISS_DestroyHandle(CAISS_HANDLE handle) {
    CAISS_ASSERT_ENVIRONMENT_INIT
    return g_manage->destroyHandle(handle);
}
