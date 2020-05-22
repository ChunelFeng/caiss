#ifndef CHUNEL_ANN_LIBRARY_H
#define CHUNEL_ANN_LIBRARY_H

#define _CNL_ANN_EXPORTS

#ifndef ANN_LIB_API
    #if defined(_CNL_ANN_EXPORTS)
        #define ANN_LIB_API __declspec(dllexport)
    #else
        #define ANN_LIB_API __declspec(dllimport)
    #endif
#endif

#include "../algorithm/hnsw/hnswlib.h"
#include "ChunelAnnLibDefine.h"

using namespace hnswlib;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


    ANN_LIB_API ANN_RET_TYPE __stdcall CNL_ANN_Environment();
    // ANN_LIB_API ANN_RET_TYPE __stdcall CNL_ANN_CreateInstance(void** handle);
    // ANN_LIB_API ANN_RET_TYPE __stdcall CNL_ANN_LoadModel(void* handle, char* path);
    //ANN_LIB_API ANN_RET_TYPE __stdcall CNL_ANN_Predict(void* handle, ANN_FLOAT* input_data, ANN_UINT top_k, char* output);
    //ANN_LIB_API ANN_RET_TYPE __stdcall CNL_ANN_DestroyInstance(void* handle);

#ifdef __cplusplus
}
#endif    // __cplusplus

#endif //CHUNELANN_LIBRARY_H
