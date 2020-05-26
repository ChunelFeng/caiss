#include "ChunelAnnLib.h"

#include "../annManageCtrl/AnnManageInclude.h"

ANN_LIB_API ANN_RET_TYPE __stdcall CNL_ANN_Environment()
{
    AnnManageProc *ann = new SyncManageProc(5);
    return ANN_RET_OK;
}