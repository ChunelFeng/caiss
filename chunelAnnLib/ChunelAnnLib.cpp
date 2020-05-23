#include "ChunelAnnLib.h"

using namespace hnswlib;

ANN_LIB_API ANN_RET_TYPE __stdcall CNL_ANN_Environment()
{
    HnswProc* a = new HnswProc();
    return ANN_RET_OK;
}