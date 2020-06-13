#ifndef _CHUNEL_CAISS_LIBRARY_H_
#define _CHUNEL_CAISS_LIBRARY_H_

#include "CaissLibDefine.h"
#include "../algorithmCtrl/AlgorithmInclude.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

    /**
     * 初始化环境信息
     * @param maxSize
     * @param algoType
     * @param manageType
     * @return
     */
    CAISS_LIB_API CAISS_RET_TYPE STDCALL CAISS_Environment(unsigned int maxSize,
            const CAISS_ALGO_TYPE algoType,
            const CAISS_MANAGE_TYPE manageType);


    /**
     *
     * @param handle
     * @return
     */
    CAISS_LIB_API CAISS_RET_TYPE STDCALL CAISS_CreateHandle(void** handle);


    /**
     *
     * @param handle
     * @param mode
     * @param distanceType
     * @param dim
     * @param modelPath
     * @param func
     * @return
     */
    CAISS_LIB_API CAISS_RET_TYPE STDCALL CAISS_Init(void *handle,
            const CAISS_MODE mode,
            const CAISS_DISTANCE_TYPE distanceType,
            const unsigned int dim,
            const char *modelPath,
            const CAISS_DIST_FUNC func);



    /**
     *
     * @param handle
     * @param dataPath
     * @param maxDataSize
     * @param normalize
     * @param maxIndexSize
     * @param precision
     * @param fastRank
     * @param realRank
     * @param step
     * @param maxEpoch
     * @param showSpan
     * @return
     */
    CAISS_LIB_API CAISS_RET_TYPE STDCALL CAISS_Train(void *handle,  const char *dataPath, const unsigned int maxDataSize, const CAISS_BOOL normalize,
                                                     const unsigned int maxIndexSize, const float precision, const unsigned int fastRank,
                                                     const unsigned int realRank, const unsigned int step, const unsigned int maxEpoch,
                                                     const unsigned int showSpan);


    /**
     *
     * @param handle
     * @param info
     * @param searchType
     * @param topK
     * @return
     */
    CAISS_LIB_API CAISS_RET_TYPE STDCALL CAISS_Search(void *handle,
            void *info,
            const CAISS_SEARCH_TYPE searchType,
            const unsigned int topK);


    /**
     *
     * @param handle
     * @param size
     * @return
     */
    CAISS_LIB_API CAISS_RET_TYPE STDCALL CAISS_getResultSize(void *handle,
            unsigned int &size);


    /**
     *
     * @param handle
     * @param result
     * @param size
     * @return
     */
    CAISS_LIB_API CAISS_RET_TYPE STDCALL CAISS_getResult(void *handle,
            char *result,
            unsigned int size);


    /**
     *
     * @param handle
     * @return
     */
    CAISS_LIB_API CAISS_RET_TYPE STDCALL CAISS_destroyHandle(void *handle);

#ifdef __cplusplus
}
#endif    // __cplusplus

#endif //_CHUNEL_CAISS_LIBRARY_H_
