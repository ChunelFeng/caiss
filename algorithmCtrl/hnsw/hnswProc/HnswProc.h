//
// Created by Chunel on 2020/5/23.
// 改编自：https://github.com/nmslib/hnswlib
//

#ifndef CAISS_HNSWPROC_H
#define CAISS_HNSWPROC_H

#include <list>
#include <./boost/bimap/bimap.hpp>
// #include <immintrin.h>    // 兼容m1版本mac，删除此处

#include "../hnswAlgo/hnswlib.h"
#include "../../AlgorithmProc.h"
#include "./HnswProcDefine.h"


class HnswProc : public AlgorithmProc {

public:
    explicit HnswProc();
    ~HnswProc() override;

    CAISS_STATUS init(CAISS_MODE mode, CAISS_DISTANCE_TYPE distanceType,
                      unsigned int dim, const char *modelPath, CAISS_DIST_FUNC distFunc) override;

    // train_mode
    CAISS_STATUS train(const char *dataPath, unsigned int maxDataSize, CAISS_BOOL normalize,
                       unsigned int maxIndexSize, float precision, unsigned int fastRank,
                       unsigned int realRank, unsigned int step, unsigned int maxEpoch,
                       unsigned int showSpan) override;

    // process_mode
    CAISS_STATUS search(void *info, CAISS_SEARCH_TYPE searchType, unsigned int topK, unsigned int filterEditDistance, CAISS_SEARCH_CALLBACK searchCBFunc, const void *cbParams) override;
    CAISS_STATUS insert(CAISS_FLOAT *node, const char *index, CAISS_INSERT_TYPE insertType) override;
    CAISS_STATUS save(const char *modelPath) override;    // 默认写成是当前模型的

protected:
    CAISS_STATUS reset();
    CAISS_STATUS loadDatas(const char *dataPath, std::vector<CaissDataNode> &datas);
    CAISS_STATUS trainModel(std::vector<CaissDataNode> &datas, unsigned int curEpoch, unsigned int maxEpoch, unsigned int showSpan);
    CAISS_STATUS buildResult(unsigned int topK, CAISS_SEARCH_TYPE searchType, const ALGO_WORD2RESULT_MAP &word2ResultMap);
    CAISS_STATUS loadModel();
    CAISS_STATUS createDistancePtr(CAISS_DIST_FUNC distFunc);
    CAISS_STATUS innerSearchResult(void *info, CAISS_SEARCH_TYPE searchType, unsigned int topK,
                                   unsigned int filterEditDistance,
                                   ALGO_WORD2RESULT_MAP& word2ResultMap);


    // 静态成员变量
private:
    static CAISS_STATUS createHnswSingleton(SpaceInterface<CAISS_FLOAT> *distance_ptr, unsigned int maxDataSize, CAISS_BOOL normalize, unsigned int maxIndexSize=64,
                                            unsigned int maxNeighbor=32, unsigned int efSearch=100, unsigned int efConstruction=100);
    static CAISS_STATUS createHnswSingleton(SpaceInterface<CAISS_FLOAT> *distance_ptr, const std::string &modelPath);
    static CAISS_STATUS destroyHnswSingleton();
    static CAISS_STATUS checkModelPrecisionEnable(float targetPrecision, unsigned int fastRank, unsigned int realRank,
                                                  const std::vector<CaissDataNode> &datas, float &calcPrecision);

    static HierarchicalNSW<CAISS_FLOAT>* getHnswSingleton();
    static CAISS_STATUS insertByOverwrite(CAISS_FLOAT *node, unsigned int label, const char *index);
    static CAISS_STATUS insertByDiscard(CAISS_FLOAT *node, unsigned int label, const char *index);

    static HierarchicalNSW<CAISS_FLOAT>*     hnsw_algo_ptr_;
    static RWLock                            hnsw_algo_lock_;

private:
    SpaceInterface<CAISS_FLOAT>*             distance_ptr_;    // 其实，这里也可以考虑用static了
    unsigned int                             neighbors_;
};


#endif //CAISS_HNSWPROC_H
