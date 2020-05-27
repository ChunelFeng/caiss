//
// Created by Chunel on 2020/5/23.
//

#ifndef CHUNELANN_HNSWPROC_H
#define CHUNELANN_HNSWPROC_H


#include "../hnswAlgo/hnswlib.h"
#include "../../AlgorithmProc.h"


using namespace hnswlib;

class HnswProc : public AlgorithmProc {

public:
    HnswProc();
    virtual ~HnswProc();

    ANN_RET_TYPE init(const ANN_MODE mode, const unsigned int dim, const char* modelPath, const unsigned int exLen);
    ANN_RET_TYPE deinit();

    // train_mode
    ANN_RET_TYPE train(const char* dataPath,
                       const unsigned int maxDataSize, const ANN_BOOL normalize, const float precision,
                       const unsigned int fastRank, const unsigned int realRank,
                       const unsigned int step, const unsigned int maxEpoch, const unsigned int showSpan);

    // process_mode
    ANN_RET_TYPE search(const ANN_FLOAT *query, const unsigned int topK, const ANN_SEARCH_TYPE searchType);
    ANN_RET_TYPE insert(const ANN_FLOAT *node, const char *label, const ANN_INSERT_TYPE insertType);   // label 是数据标签
    ANN_RET_TYPE save(const char *modelPath);    // 默认写成是当前模型的
    ANN_RET_TYPE getResultSize(unsigned int& size);
    ANN_RET_TYPE getResult(char *result, unsigned int size);
    ANN_RET_TYPE ignore(const char *label);


protected:
    ANN_RET_TYPE resetHnswProcMember();
    ANN_RET_TYPE loadDatas(const char *dataPath, std::vector<ANN_VECTOR_FLOAT> &datas);
    ANN_RET_TYPE trainModel(std::vector<ANN_VECTOR_FLOAT> &datas);
    ANN_RET_TYPE buildResult(const ANN_FLOAT *query, const std::vector<unsigned int> &predIndex);


private:
    L2Space*                    l2s_ptr_;
    HierarchicalNSW<ANN_FLOAT>* hnsw_alg_ptr_;    // 考虑这里用static信息
    RapidJsonProc*              json_proc_;

};


#endif //CHUNELANN_HNSWPROC_H
