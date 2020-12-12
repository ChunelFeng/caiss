/***************************
@Author: Chunel
@Contact: chunel@foxmail.com
@File: MrptProcDefine.h
@Time: 2020/12/5 2:48 下午
@Desc: 
***************************/


#ifndef CAISS_MRPTPROCDEFINE_H
#define CAISS_MRPTPROCDEFINE_H

#include <vector>
#include <string>

struct MrptModelHead {
    explicit MrptModelHead() {
        reset();
    }

    explicit MrptModelHead(int maxDataSize, int dim) {
        this->modelTag = 0;
        this->maxDataSize = maxDataSize;
        this->dim = dim;
        this->maxIndexSize = 0;
        this->curSize = 0;
        this->modelData.clear();
        this->labels.clear();
    }

    virtual ~MrptModelHead() {
        reset();
    }

    long calcSkipSize() const {
        long size = 5*sizeof(int)    // 数值字段大小
                + sizeof(float)*curSize*dim    // 向量信息大小
                + maxIndexSize*curSize;    // label信息大小
        return size;
    }

    int modelTag;
    int maxDataSize;
    int dim;
    int maxIndexSize;
    int curSize;    // 记住，存放的是当前真实的size，不是maxSize
    std::vector<float> modelData;
    std::vector<std::string> labels;

protected:
    void reset() {
        this->modelTag = 0;
        this->maxDataSize = 0;
        this->dim = 0;
        this->maxIndexSize = 0;
        this->curSize = 0;
        this->modelData.clear();
        this->labels.clear();
    }
};


#endif //CAISS_MRPTPROCDEFINE_H
