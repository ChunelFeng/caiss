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
    ANN_RET_TYPE init(ANN_MODE mode, unsigned int dim, char *modelPath, unsigned int exLen);
    ANN_RET_TYPE deinit();

    // train_mode
    ANN_RET_TYPE train(char* dataPath,
            unsigned int maxDataSize, ANN_BOOL normalize, float precision, unsigned int fastRank, unsigned int realRank,
            unsigned int step, unsigned int maxEpoch, unsigned int showSpan);

    // process_mode
    ANN_RET_TYPE search(ANN_FLOAT* query, unsigned int topK, ANN_SEARCH_TYPE searchType);
    ANN_RET_TYPE insert(ANN_FLOAT* node, char* label, ANN_INSERT_TYPE insertType);   // label 是数据标签
    ANN_RET_TYPE save(char* modePah);    // 默认写成是当前模型的
    ANN_RET_TYPE getResultSize(unsigned int& size);
    ANN_RET_TYPE getResult(char* result, unsigned int size);

protected:
    virtual ~HnswProc();
    ANN_RET_TYPE resetMemberViriables();

    ANN_RET_TYPE loadDatas(std::vector<ANN_VECTOR_FLOAT> &datas);

    ANN_RET_TYPE trainModel(const std::vector<ANN_VECTOR_FLOAT> datas);


private:
    L2Space*                    l2s_ptr_;
    HierarchicalNSW<ANN_FLOAT>* hnsw_alg_ptr_;
    RapidJsonProc*              json_proc_;

};


#endif //CHUNELANN_HNSWPROC_H
