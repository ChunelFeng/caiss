//
// Created by Chunel on 2020/6/13.
//

#ifndef CAISS_HNSWPROCDEFINE_H
#define CAISS_HNSWPROCDEFINE_H

const static unsigned int NEIGHBOR_NUMS_DEFAULT = 32;
const static unsigned int EF_SEARCH_DEFAULT = 100;
const static unsigned int EF_CONSTRUCTOR_DEFAULT = 100;

struct HnswTrainParams {
    HnswTrainParams(unsigned int step) {
        this->neighborNums = NEIGHBOR_NUMS_DEFAULT;
        this->efSearch = EF_SEARCH_DEFAULT;
        this->efConstructor = EF_CONSTRUCTOR_DEFAULT;
        this->step = step;
    }

    HnswTrainParams(unsigned int neighborNums, unsigned int efSearch, unsigned int efConstructor, unsigned int step) {
        this->neighborNums = neighborNums;
        this->efSearch = efSearch;
        this->efConstructor = efConstructor;
        this->step = step;
    }

    void update(float span) {
        // 传入的是精确度的差距
        this->neighborNums += (unsigned int)((float)this->neighborNums * (1.0f + span * 10.0f) * (float)step / 10.0f);
        this->efConstructor += (unsigned int)((float)this->efConstructor * (1.0f + span * 5.0f) * (float)step / 5.0f);
        this->efSearch = this->efConstructor;
    }

    unsigned int neighborNums;      // 最大邻居树
    unsigned int efSearch;          // 查询比较点数
    unsigned int efConstructor;     // 构造比较点数
    unsigned int step;              // 数据更新快慢的决定因素
};

#endif //CAISS_HNSWPROCDEFINE_H
