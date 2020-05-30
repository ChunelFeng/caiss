#include "ChunelAnnLib.h"

#include "../annManageCtrl/AnnManageInclude.h"

static AnnManageProc* g_manage = nullptr;

ANN_LIB_API ANN_RET_TYPE __stdcall CNL_ANN_Environment(unsigned int maxSize)
{
    if (nullptr == g_manage) {
        g_manage = new SyncManageProc(maxSize);    // 暂时只做同步的版本
    }
    return ANN_RET_OK;
}

ANN_LIB_API ANN_RET_TYPE __stdcall CNL_ANN_CreateHandle(void** handle) {
    if (nullptr == g_manage) {
        return ANN_RET_RES;
    }

}