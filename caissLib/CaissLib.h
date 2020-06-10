#ifndef _CHUNEL_CAISS_LIBRARY_H_
#define _CHUNEL_CAISS_LIBRARY_H_

#ifndef CAISS_LIB_API
    #if defined(_CNL_CAISS_EXPORTS)
        #define CAISS_LIB_API __declspec(dllexport)
    #else
        #define CAISS_LIB_API __declspec(dllimport)
    #endif
#endif

#ifdef WIN32
    #define STDCALL __stdcall
#else
    #define STDCALL
#endif

#include "CaissLibDefine.h"
#include "../algorithmCtrl/AlgorithmInclude.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

    CAISS_LIB_API CAISS_RET_TYPE STDCALL CAISS_Environment(unsigned int maxSize,
            const CAISS_ALGO_TYPE algoType, const CAISS_MANAGE_TYPE manageType);

    //CAISS_LIB_API CAISS_RET_TYPE __stdcall CAISS_CreateHandle(void** handle);

#ifdef __cplusplus
}
#endif    // __cplusplus

#endif //_CHUNEL_CAISS_LIBRARY_H_
