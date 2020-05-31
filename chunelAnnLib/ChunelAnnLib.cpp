#include "ChunelAnnLib.h"

#include "../annManageCtrl/AnnManageInclude.h"

static AnnManageProc* g_manage = nullptr;
static ANN_BOOL g_init = ANN_FALSE;

ANN_LIB_API ANN_RET_TYPE __stdcall CNL_ANN_Environment(unsigned int maxSize,
                                                       const ANN_ALGO_TYPE algoType, const ANN_MANAGE_TYPE manageType);
{
    if (nullptr == g_manage) {
        g_manage = new SyncManageProc(0, ANN_ALGO_NSG, maxSize, ANN_ALGO_HNSW);    // 暂时只做同步的版本
        g_init = ANN_TRUE;
    }
    return ANN_RET_OK;
}

ANN_LIB_API ANN_RET_TYPE __stdcall CNL_ANN_CreateHandle(void** handle) {
    if (nullptr == g_manage) {
        return ANN_RET_RES;
    }

}