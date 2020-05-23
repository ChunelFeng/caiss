#ifndef _CHUNEL_ANN_LIBRARY_H_
#define _CHUNEL_ANN_LIBRARY_H_

   #define _CNL_ANN_EXPORTS    // 这里今后要给删除掉

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

    ANN_LIB_API ANN_RET_TYPE __stdcall CNL_ANN_Environment();
//    ANN_LIB_API ANN_RET_TYPE __stdcall CNL_ANN_CreateInstance(void** handle);
//    ANN_LIB_API ANN_RET_TYPE __stdcall CNL_ANN_InitInsert();
//    ANN_LIB_API ANN_RET_TYPE __stdcall CNL_ANN_Insert();

#ifdef __cplusplus
}
#endif    // __cplusplus

#endif //_CHUNEL_ANN_LIBRARY_H_
