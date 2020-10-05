//
// Created by Chunel on 2020/10/5.
//

#ifndef CAISS_CPPCAISS_H
#define CAISS_CPPCAISS_H

#include "cppCaissDefine.h"


class cppCaiss {

public:
    cppCaiss();
    virtual ~cppCaiss();

    static int Environment(unsigned int maxThreadSize,
                           CAISS_ALGO_TYPE algoType,
                           CAISS_MANAGE_TYPE manageType) {

    }

    int CreateHandle(void **handle){

    }

    int Init(void *handle,
             CAISS_MODE mode,
             CAISS_DISTANCE_TYPE distanceType,
             unsigned int dim,
             const char *modelPath,
             CAISS_DIST_FUNC distFunc = nullptr) {

    }

    int Train(void *handle,
              const char *dataPath,
              unsigned int maxDataSize,
              CAISS_BOOL normalize,
              unsigned int maxIndexSize = 64,
              float precision = 0.95,
              unsigned int fastRank = 5,
              unsigned int realRank = 5,
              unsigned int step = 1,
              unsigned int maxEpoch = 5,
              unsigned int showSpan = 1000) {

    }

    int Search(void *handle,
               void *info,
               CAISS_SEARCH_TYPE searchType,
               unsigned int topK,
               unsigned int filterEditDistance = CAISS_DEFAULT_EDIT_DISTANCE,
               CAISS_SEARCH_CALLBACK searchCBFunc = nullptr,
               const void *cbParams = nullptr) {

    }

    int GetResultSize(void *handle,
                      unsigned int &size) {

    }

    int GetResult(void *handle,
                  char *result,
                  unsigned int size) {

    }

    int Insert(void *handle,
               CAISS_FLOAT *node,
               const char *label,
               CAISS_INSERT_TYPE insertType) {

    }

    int Ignore(void *handle,
               const char *label,
               CAISS_BOOL isIgnore = CAISS_TRUE) {

    }

    int Save(void *handle,
             const char *modelPath = nullptr) {

    }

    int ExecuteSQL(void *handle,
                   const char *sql,
                   CAISS_SQL_CALLBACK sqlCBFunc = nullptr,
                   const void *sqlParams = nullptr) {

    }

    int DestroyHandle(void *handle) {

    }
};



#endif //CAISS_CPPCAISS_H
