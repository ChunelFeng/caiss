#include "CaissLib.h"

#include "../manageCtrl/manageInclude.h"

static manageProc* g_manage = nullptr;
static CAISS_BOOL g_init = CAISS_FALSE;

CAISS_LIB_API CAISS_RET_TYPE __stdcall CAISS_Environment(unsigned int maxSize,
                                                     const CAISS_ALGO_TYPE algoType, const CAISS_MANAGE_TYPE manageType)
{
    if (nullptr == g_manage) {
        g_manage = new SyncManageProc(maxSize, algoType);    // 暂时只做同步的版本
        g_init = CAISS_TRUE;
    }
    return CAISS_RET_OK;
}

manageProc* getManageSingleton() {

    return nullptr;
}