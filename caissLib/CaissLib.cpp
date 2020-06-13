#include "CaissLib.h"

#include "../manageCtrl/manageInclude.h"

#include <atomic>

static manageProc* g_manage = nullptr;
static std::atomic<CAISS_BOOL> g_init(CAISS_FALSE);
static RWLock g_lock;

CAISS_LIB_API CAISS_RET_TYPE STDCALL CAISS_Environment(const unsigned int maxSize,
                                                       const CAISS_ALGO_TYPE algoType,
                                                       const CAISS_MANAGE_TYPE manageType) {
    CAISS_FUNCTION_BEGIN
    if (nullptr == g_manage) {
        g_lock.writeLock();
        if (nullptr == g_manage) {
            g_manage = new SyncManageProc(maxSize, algoType);    // 暂时只做同步的版本
            g_init = CAISS_TRUE;    // 通过init参数，来确定环境是否初始化。如果初始化了，则
        }
        g_lock.writeUnlock();
    }

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
                                                const CAISS_DIST_FUNC func) {
    CAISS_ASSERT_ENVIRONMENT_INIT
    return g_manage->init(handle, mode, distanceType, dim, modelPath, func);
}


CAISS_LIB_API CAISS_RET_TYPE STDCALL CAISS_Search(void *handle,
                                                  void *info,
                                                  const CAISS_SEARCH_TYPE searchType,
                                                  const unsigned int topK) {
    CAISS_ASSERT_ENVIRONMENT_INIT
    return g_manage->search(handle, info, searchType, topK);

}

CAISS_LIB_API CAISS_RET_TYPE STDCALL CAISS_getResultSize(void *handle,
                                                         unsigned int &size) {
    CAISS_ASSERT_ENVIRONMENT_INIT
    return g_manage->getResultSize(handle, size);
}

CAISS_LIB_API CAISS_RET_TYPE STDCALL CAISS_getResult(void *handle,
                                                     char *result,
                                                     unsigned int size) {
    CAISS_ASSERT_ENVIRONMENT_INIT
    return g_manage->getResult(handle, result, size);
}

CAISS_LIB_API CAISS_RET_TYPE STDCALL CAISS_destroyHandle(void *handle) {
    CAISS_ASSERT_ENVIRONMENT_INIT
    return g_manage->destroyHandle(handle);
}
