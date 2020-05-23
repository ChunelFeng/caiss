//
// Created by Chunel on 2020/5/23.
// 所有算法类的基类信息
//

#ifndef CHUNELANN_ALGORITHMPROC_H
#define CHUNELANN_ALGORITHMPROC_H


#include <string>
#include "../chunelAnnLib/ChunelAnnLib.h"
#include "../utilsCtrl/UtilsInclude.h"


const static unsigned int DEFAULT_STEP = 5;
const static unsigned int DEFAULT_MAX_EPOCH = 10;
const static unsigned int DEFAULT_SHOW_SPAN = 100;    // 100行会显示一次日志

class AlgorithmProc {

public:
    AlgorithmProc() {
        resetMemberViriables();
    }

    virtual ANN_RET_TYPE init(ANN_MODE mode, unsigned int dim, char *modelPath, unsigned int exLen) = 0;
    virtual ANN_RET_TYPE deinit() = 0;

    // train_mode
    virtual ANN_RET_TYPE train(char* dataPath,
            unsigned int maxDataSize, ANN_BOOL normalize, float precision, unsigned int fastRank, unsigned int realRank,
            unsigned int step = DEFAULT_STEP, unsigned int maxEpoch = DEFAULT_MAX_EPOCH, unsigned int showSpan = DEFAULT_SHOW_SPAN) = 0;

    // process_mode
    virtual ANN_RET_TYPE search(ANN_FLOAT* query, unsigned int topK, ANN_SEARCH_TYPE searchType = ANN_SEARCH_FAST) = 0;
    virtual ANN_RET_TYPE insert(ANN_FLOAT* node, char* label, ANN_INSERT_TYPE insertType = ANN_INSERT_ADD) = 0;   // label 是数据标签
    virtual ANN_RET_TYPE save(char* modePah = nullptr) = 0;    // 默认写成是当前模型的
    virtual ANN_RET_TYPE getResultSize(unsigned int& size) = 0;
    virtual ANN_RET_TYPE getResult(char* result, unsigned int size) = 0;

    virtual ANN_RET_TYPE resetMemberViriables() {
        this->dim_ = 0;
        this->cur_mode_ = ANN_MODE_DEFAULT;
        this->normalize_ = ANN_FALSE;
        this->model_path_ptr_ = nullptr;

        ANN_FUNCTION_END
    }

protected:
    virtual ~AlgorithmProc() {
        resetMemberViriables();
    }

    ANN_RET_TYPE normalizeNode(ANN_FLOAT *node) {
        ANN_ASSERT_NOT_NULL(node)

        ANN_FLOAT sum = 0.0;
        for (unsigned int i = 0; i < this->dim_; i++) {
            sum += (node[i] * node[i]);
        }
        ANN_FLOAT denominator = fastSqrt(sum);
        for (unsigned int i = 0; i < this->dim_; i++) {
            node[i] = node[i] / denominator;
        }

        return ANN_RET_OK;
    }

    float fastSqrt(float x) {
        /* 快速开平方计算方式 */
        float xhalf = 0.5f * x;
        int i = *(int*)&x;     // get bits for floating VALUE
        i = 0x5f375a86 - (i >> 1);    // gives initial guess y0
        x = *(float*)&i;     // convert bits BACK to float
        x = x * (1.5f - xhalf * x * x);     // Newton step, repeating increases accuracy
        return 1 / x;
    }



protected:
    char* model_path_ptr_;
    unsigned int dim_;
    ANN_MODE cur_mode_;
    ANN_BOOL normalize_;    // 是否需要标准化数据
};

#endif //CHUNELANN_ALGORITHMPROC_H
