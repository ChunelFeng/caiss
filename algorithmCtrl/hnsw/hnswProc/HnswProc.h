//
// Created by Chunel on 2020/5/23.
//

#ifndef CHUNELCAISS_HNSWPROC_H
#define CHUNELCAISS_HNSWPROC_H

#include <list>

#include "../hnswAlgo/hnswlib.h"
#include "../../AlgorithmProc.h"
#include "./HnswProcDefine.h"
#include <./boost/bimap/bimap.hpp>

using namespace hnswlib;

class HnswProc : public AlgorithmProc {

public:
    std::list<std::string>                 result_words_;    // todo 这里需要给删除，今后再删
    std::list<CAISS_FLOAT>                 result_distance_;    // 查找到的距离

    explicit HnswProc();
    virtual ~HnswProc();

    CAISS_RET_TYPE init(const CAISS_MODE mode, const CAISS_DISTANCE_TYPE distanceType,
                        const unsigned int dim, const char *modelPath, const CAISS_DIST_FUNC distFunc);

    // train_mode
    CAISS_RET_TYPE train(const char *dataPath, const unsigned int maxDataSize, const CAISS_BOOL normalize,
                         const unsigned int maxIndexSize, const float precision, const unsigned int fastRank,
                         const unsigned int realRank, const unsigned int step, const unsigned int maxEpoch,
                         const unsigned int showSpan);

    // process_mode
    CAISS_RET_TYPE search(void *info, const CAISS_SEARCH_TYPE searchType, const unsigned int topK);
    CAISS_RET_TYPE insert(CAISS_FLOAT *node, const char *index, CAISS_INSERT_TYPE insertType);
    CAISS_RET_TYPE save(const char *modelPath);    // 默认写成是当前模型的
    CAISS_RET_TYPE getResultSize(unsigned int& size);
    CAISS_RET_TYPE getResult(char *result, unsigned int size);
    CAISS_RET_TYPE ignore(const char *label);    // todo 暂未完成功能


protected:
    CAISS_RET_TYPE reset();
    CAISS_RET_TYPE loadDatas(const char *dataPath, std::vector<CaissDataNode> &datas);
    CAISS_RET_TYPE trainModel(std::vector<CaissDataNode> &datas, const unsigned int showSpan);
    CAISS_RET_TYPE buildResult(const CAISS_FLOAT *query, const CAISS_SEARCH_TYPE searchType,
                               std::priority_queue<std::pair<CAISS_FLOAT, labeltype>> &predResult);
    CAISS_RET_TYPE loadModel(const char *modelPath);
    CAISS_RET_TYPE createDistancePtr(CAISS_DIST_FUNC distFunc);
    CAISS_RET_TYPE innerSearchResult(void *info, CAISS_SEARCH_TYPE searchType, const unsigned int topK);
    CAISS_RET_TYPE searchInLruCache(const char *word, const CAISS_SEARCH_TYPE searchType, const unsigned int topK, CAISS_BOOL &isGet);


    // 静态成员变量
private:

    static CAISS_RET_TYPE createHnswSingleton(SpaceInterface<CAISS_FLOAT>* distance_ptr, unsigned int maxDataSize, CAISS_BOOL normalize, const unsigned int maxIndexSize=64,
                                              const unsigned int maxNeighbor=32, const unsigned int efSearch=100, const unsigned int efConstruction=100);
    static CAISS_RET_TYPE createHnswSingleton(SpaceInterface<CAISS_FLOAT>* distance_ptr, const std::string &modelPath);
    static CAISS_RET_TYPE destroyHnswSingleton();
    static CAISS_RET_TYPE checkModelPrecisionEnable(const float targetPrecision, const unsigned int fastRank, const unsigned int realRank,
                                                    const std::vector<CaissDataNode> &datas, float &calcPrecision);

    static HierarchicalNSW<CAISS_FLOAT>* getHnswSingleton();
    static CAISS_RET_TYPE insertByOverwrite(CAISS_FLOAT *node, unsigned int label, const char *index);
    static CAISS_RET_TYPE insertByDiscard(CAISS_FLOAT *node, unsigned int label, const char *index);

    static HierarchicalNSW<CAISS_FLOAT>*     hnsw_alg_ptr_;
    static RWLock                            hnsw_lock_;

private:
    SpaceInterface<CAISS_FLOAT>*             distance_ptr_;    // 其实，这里也可以考虑用static了
};


#endif //CHUNELCAISS_HNSWPROC_H
