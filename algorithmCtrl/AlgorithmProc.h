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

const static std::string MODEL_SUFFIX = ".ann";   // 默认的模型后缀


class AlgorithmProc {

public:
    AlgorithmProc() {
        resetAlgorithmProcMember();
    }

    virtual ANN_RET_TYPE init(const ANN_MODE mode, const unsigned int dim, const char *modelPath, const unsigned int exLen) = 0;
    virtual ANN_RET_TYPE deinit() = 0;

    // train_mode
    virtual ANN_RET_TYPE train(const char *dataPath,
                               const unsigned int maxDataSize, const ANN_BOOL normalize, const float precision, const unsigned int fastRank, const unsigned int realRank,
                               const unsigned int step = DEFAULT_STEP, const unsigned int maxEpoch = DEFAULT_MAX_EPOCH, const unsigned int showSpan = DEFAULT_SHOW_SPAN) = 0;

    // process_mode
    virtual ANN_RET_TYPE search(const ANN_FLOAT *query, const unsigned int topK, const ANN_SEARCH_TYPE searchType = ANN_SEARCH_FAST) = 0;
    virtual ANN_RET_TYPE insert(const ANN_FLOAT *node, const char *label, const ANN_INSERT_TYPE insertType = ANN_INSERT_ADD) = 0;   // label 是数据标签
    virtual ANN_RET_TYPE save(char* modePah = nullptr) = 0;    // 默认写成是当前模型的
    virtual ANN_RET_TYPE getResultSize(unsigned int& size) = 0;
    virtual ANN_RET_TYPE getResult(char* result, unsigned int size) = 0;

    void resetAlgorithmProcMember() {
        this->dim_ = 0;
        this->cur_mode_ = ANN_MODE_DEFAULT;
        this->normalize_ = ANN_FALSE;
        this->model_path_.clear();
    }

protected:
    virtual ~AlgorithmProc() {
        resetAlgorithmProcMember();
    }

    ANN_RET_TYPE normalizeNode(ANN_FLOAT *node) {
        ANN_ASSERT_NOT_NULL(node)

        if (ANN_FALSE == normalize_) {
            return ANN_RET_OK;    // 如果不需要归一化，直接返回
        }

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
    std::string model_path_;
    unsigned int dim_;
    ANN_MODE cur_mode_;
    ANN_BOOL normalize_;    // 是否需要标准化数据
};

#endif //CHUNELANN_ALGORITHMPROC_H
