//
// Created by Chunel on 2020/5/23.
// 所有算法类的基类信息
//

#ifndef CHUNELANN_ALGORITHMPROC_H
#define CHUNELANN_ALGORITHMPROC_H

#include "../chunelAnnLib/ChunelAnnLib.h"

#define DEFAULT_STEP (5)
#define DEFAULT_MAX_EPOCH (10)
#define DEFAULT_SHOW_SPAN (100)    // 100行会显示一次

class AlgorithmProc {
public:
    char* model_path_;
    unsigned int dim_;


public:

    virtual ANN_RET_TYPE init(ANN_MODE mode, unsigned int dim, ANN_BOOL normalize, char* modelPath) = 0;

    // train_mode
    virtual ANN_RET_TYPE train(char* dataPath,
            unsigned int dim, unsigned int maxDataSize, float precision, unsigned int fastRank, unsigned int realRank,
            unsigned int step = DEFAULT_STEP, unsigned int maxEpoch = DEFAULT_MAX_EPOCH, unsigned int showSpan = DEFAULT_SHOW_SPAN) = 0;

    // process_mode
    virtual ANN_RET_TYPE search(ANN_FLOAT* query, unsigned int topK, ANN_SEARCH_TYPE searchType = ANN_SEARCH_FAST) = 0;
    virtual ANN_RET_TYPE insert(ANN_FLOAT* node, char* label, ANN_INSERT_TYPE insertType = ANN_INSERT_ADD) = 0;   // label 是数据标签
    virtual ANN_RET_TYPE save(char* modePah = nullptr) = 0;    // 默认写成是当前模型的

    virtual ~AlgorithmProc() = default;
};


#endif //CHUNELANN_ALGORITHMPROC_H
