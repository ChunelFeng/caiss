//
// Created by Chunel on 2020/5/23.
// 所有算法类的基类信息
//

#ifndef CHUNELANN_ALGORITHMPROC_H
#define CHUNELANN_ALGORITHMPROC_H


#include <string>
#include "../chunelAnnLib/ChunelAnnLib.h"
#include "../utilsCtrl/UtilsInclude.h"
#include "../threadCtrl/ThreadInclude.h"


const static unsigned int DEFAULT_STEP = 5;
const static unsigned int DEFAULT_MAX_EPOCH = 10;
const static unsigned int DEFAULT_SHOW_SPAN = 100;    // 100行会显示一次日志
const static std::string MODEL_SUFFIX = ".caiss";   // 默认的模型后缀


class AlgorithmProc {

public:
    explicit AlgorithmProc() {
    }

    virtual ~AlgorithmProc() {
    }


    /**
     * 初始化状态和参数信息
     * @param mode
     * @param distanceType
     * @param dim
     * @param modelPath
     * @param exLen
     * @return
     */
    virtual ANN_RET_TYPE init(const ANN_MODE mode, const ANN_DISTANCE_TYPE distanceType, const unsigned int dim, const char *modelPath,
                              const unsigned int exLen) = 0;

    // train_mode
    /**
     * 开始精确训练方法
     * @param dataPath
     * @param maxDataSize
     * @param normalize
     * @param precision
     * @param fastRank
     * @param realRank
     * @param step
     * @param maxEpoch
     * @param showSpan
     * @return
     */
    virtual ANN_RET_TYPE train(const char *dataPath,
                               const unsigned int maxDataSize, const ANN_BOOL normalize, const float precision, const unsigned int fastRank, const unsigned int realRank,
                               const unsigned int step = DEFAULT_STEP, const unsigned int maxEpoch = DEFAULT_MAX_EPOCH, const unsigned int showSpan = DEFAULT_SHOW_SPAN) = 0;

    // process_mode
    /**
     * 查询topK个距离query最近的结果
     * @param info
     * @param topK
     * @param searchType
     * @return
     */
    virtual ANN_RET_TYPE search(void *info, ANN_SEARCH_TYPE searchType, const unsigned int topK) = 0;

    /**
     * 插入结果信息
     * @param node
     * @param index
     * @param insertType
     * @return
     */
    virtual ANN_RET_TYPE insert(ANN_FLOAT *node, const char *index, const ANN_INSERT_TYPE insertType = ANN_INSERT_OVERWRITE) = 0;   // label 是数据标签

    /**
     * 保存模型信息
     * @param modelPath
     * @return
     */
    virtual ANN_RET_TYPE save(const char *modelPath = nullptr) = 0;    // 默认写成是当前模型的

    /**
     * 获取结果的长度
     * @param size
     * @return
     */
    virtual ANN_RET_TYPE getResultSize(unsigned int& size) = 0;

    /**
     * 获取结果
     * @param result
     * @param size
     * @return
     */
    virtual ANN_RET_TYPE getResult(char *result, unsigned int size) = 0;

    /**
     * 被忽略的节点
     * @param label
     * @return
     */
    virtual ANN_RET_TYPE ignore(const char *label) = 0;

protected:

    ANN_RET_TYPE normalizeNode(std::vector<ANN_FLOAT>& node, unsigned int dim) {
        if (ANN_FALSE == this->normalize_) {
            return ANN_RET_OK;    // 如果不需要归一化，直接返回
        }

        if (dim != this->dim_) {
            return ANN_RET_DIM;    // 忽略维度不一致的情况
        }

        ANN_FLOAT sum = 0.0;
        for (unsigned int i = 0; i < this->dim_; i++) {
            sum += (node[i] * node[i]);
        }

        ANN_FLOAT denominator = std::sqrt(sum);    // 分母信息
        for (unsigned int i = 0; i < this->dim_; i++) {
            node[i] = node[i] / denominator;
        }

        return ANN_RET_OK;
    }

//    ANN_RET_TYPE normalizeNode(ANN_FLOAT *node, unsigned int dim) {
//        ANN_ASSERT_NOT_NULL(node);
//
//        if (ANN_FALSE == this->normalize_) {
//            return ANN_RET_OK;    // 如果不需要归一化，直接返回
//        }
//
//        if (dim != this->dim_) {
//            return ANN_RET_DIM;    // 忽略维度不一致的情况
//        }
//
//        ANN_FLOAT sum = 0.0;
//        for (unsigned int i = 0; i < this->dim_; i++) {
//            sum += (node[i] * node[i]);
//        }
//
//        ANN_FLOAT denominator = std::sqrt(sum);    // 分母信息
//        for (unsigned int i = 0; i < this->dim_; i++) {
//            node[i] = node[i] / denominator;
//        }
//
//        return ANN_RET_OK;
//    }

    float fastSqrt(float x) {
        /* 快速开平方计算方式 */
        float half = 0.5f * x;
        int i = *(int*)&x;     // get bits for floating VALUE
        i = 0x5f375a86 - (i >> 1);    // gives initial guess y0
        x = *(float*)&i;     // convert bits BACK to float
        x = x * (1.5f - half * x * x);     // Newton step, repeating increases accuracy
        return 1 / x;
    }


protected:
    std::string model_path_;
    unsigned int dim_;
    ANN_MODE cur_mode_;
    ANN_BOOL normalize_;    // 是否需要标准化数据
    std::string result_;
    ANN_DISTANCE_TYPE distance_type_;
};

#endif //CHUNELANN_ALGORITHMPROC_H
