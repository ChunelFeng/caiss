//
// Created by Chunel on 2020/5/23.
//

#ifndef CHUNELANN_HNSWPROC_H
#define CHUNELANN_HNSWPROC_H

#include "../../AlgorithmProc.h"
#include "../hnswAlgo/hnswlib.h"

using namespace hnswlib;

class hnswProc : public AlgorithmProc {

    // 这里的模型，应该是一个单例内容。多个Proc的handle，对应一个模型的内容
    // 需要实现AlgorithmProc类的各个内容
};


#endif //CHUNELANN_HNSWPROC_H
