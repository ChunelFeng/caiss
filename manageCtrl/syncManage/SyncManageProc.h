//
// Created by Chunel on 2020/5/24.
// 同步管理的逻辑处理
//

#ifndef CAISS_SYNCMANAGEPROC_H
#define CAISS_SYNCMANAGEPROC_H

#include "../ManageProc.h"

class SyncManageProc : public ManageProc  {
public:
    SyncManageProc(unsigned int maxSize, CAISS_ALGO_TYPE algoType) : ManageProc(maxSize, algoType) {
    }

    ~SyncManageProc() override = default;

    CAISS_RET_TYPE train(void *handle, const char *dataPath, unsigned int maxDataSize, CAISS_BOOL normalize,
                         unsigned int maxIndexSize, float precision, unsigned int fastRank,
                         unsigned int realRank, unsigned int step, unsigned int maxEpoch,
                         unsigned int showSpan) override ;

    CAISS_RET_TYPE search(void *handle, void *info, CAISS_SEARCH_TYPE searchType, unsigned int topK, unsigned int filterEditDistance, CAISS_SEARCH_CALLBACK searchCBFunc, const void *cbParams) override ;
    CAISS_RET_TYPE getResultSize(void *handle, unsigned int &size) override ;
    CAISS_RET_TYPE getResult(void *handle, char *result, unsigned int size) override ;

    CAISS_RET_TYPE save(void *handle, const char *modelPath) override ;
    // label 是数据标签，index表示数据第几个信息
    CAISS_RET_TYPE insert(void *handle, CAISS_FLOAT *node, const char *label, CAISS_INSERT_TYPE insertType) override ;
    CAISS_RET_TYPE ignore(void *handle, const char *label, CAISS_BOOL isIgnore) override ;

    // 暂时仅支持同步模式
    CAISS_RET_TYPE executeSQL(void *handle,
            const char *sql,
            CAISS_SEARCH_CALLBACK sqlCBFunc = nullptr,
            const void *sqlParams = nullptr) override ;
};


#endif //CAISS_SYNCMANAGEPROC_H
