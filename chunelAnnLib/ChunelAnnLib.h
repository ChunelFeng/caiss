#ifndef _CHUNEL_ANN_LIBRARY_H_
#define _CHUNEL_ANN_LIBRARY_H_

#ifndef ANN_LIB_API
    #if defined(_CNL_ANN_EXPORTS)
        #define ANN_LIB_API __declspec(dllexport)
    #else
        #define ANN_LIB_API __declspec(dllimport)
    #endif
#endif

#include "ChunelAnnLibDefine.h"
#include "../algorithmCtrl/AlgorithmInclude.h"


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

    ANN_LIB_API ANN_RET_TYPE __stdcall CNL_ANN_Environment(unsigned int maxSize,
            const ANN_ALGO_TYPE algoType, const ANN_MANAGE_TYPE manageType);

    ANN_LIB_API ANN_RET_TYPE __stdcall CNL_ANN_CreateHandle(void** handle);

#ifdef __cplusplus
}
#endif    // __cplusplus

#endif //_CHUNEL_ANN_LIBRARY_H_
