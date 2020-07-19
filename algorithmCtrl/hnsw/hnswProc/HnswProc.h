//
// Created by Chunel on 2020/5/23.
//

#ifndef CHUNELCAISS_HNSWPROC_H
#define CHUNELCAISS_HNSWPROC_H

#include <list>
#include <./boost/bimap/bimap.hpp>

#include "../hnswAlgo/hnswlib.h"
#include "../../AlgorithmProc.h"
#include "./HnswProcDefine.h"


using namespace hnswlib;
using HNSW_RET_TYPE = std::priority_queue<std::pair<CAISS_FLOAT, labeltype>>;

class HnswProc : public AlgorithmProc {

public:
    std::list<std::string>                 result_words_;
    std::list<CAISS_FLOAT>                 result_distance_;    // 查找到的距离

    explicit HnswProc();
    ~HnswProc() override;

    CAISS_RET_TYPE init(CAISS_MODE mode, CAISS_DISTANCE_TYPE distanceType,
                        unsigned int dim, const char *modelPath, CAISS_DIST_FUNC distFunc) override;

    // train_mode
    CAISS_RET_TYPE train(const char *dataPath, unsigned int maxDataSize, CAISS_BOOL normalize,
                         unsigned int maxIndexSize, float precision, unsigned int fastRank,
                         unsigned int realRank, unsigned int step, unsigned int maxEpoch,
                         unsigned int showSpan) override;

    // process_mode
    CAISS_RET_TYPE search(void *info, CAISS_SEARCH_TYPE searchType, unsigned int topK, unsigned int filterEditDistance, CAISS_SEARCH_CALLBACK searchCBFunc, const void *cbParams) override;
    CAISS_RET_TYPE insert(CAISS_FLOAT *node, const char *index, CAISS_INSERT_TYPE insertType) override;
    CAISS_RET_TYPE save(const char *modelPath) override;    // 默认写成是当前模型的
    CAISS_RET_TYPE getResultSize(unsigned int& size) override;
    CAISS_RET_TYPE getResult(char *result, unsigned int size) override;
    CAISS_RET_TYPE ignore(const char *label, bool isIgnore) override;


protected:
    CAISS_RET_TYPE reset();
    CAISS_RET_TYPE loadDatas(const char *dataPath, std::vector<CaissDataNode> &datas);
    CAISS_RET_TYPE trainModel(std::vector<CaissDataNode> &datas, unsigned int showSpan);
    CAISS_RET_TYPE buildResult(const CAISS_FLOAT *query, CAISS_SEARCH_TYPE searchType,
                               HNSW_RET_TYPE &predResult);
    CAISS_RET_TYPE loadModel(const char *modelPath);
    CAISS_RET_TYPE createDistancePtr(CAISS_DIST_FUNC distFunc);
    CAISS_RET_TYPE innerSearchResult(void *info, CAISS_SEARCH_TYPE searchType, unsigned int topK,
                                    unsigned int filterEditDistance);
    CAISS_RET_TYPE searchInLruCache(const char *word, CAISS_SEARCH_TYPE searchType, unsigned int topK, CAISS_BOOL &isGet);

    /* 函数过滤条件 */
    CAISS_RET_TYPE filterByRules(void *info, CAISS_SEARCH_TYPE searchType, HNSW_RET_TYPE &result, unsigned int topK,
                                 unsigned int filterEditDistance);
    CAISS_RET_TYPE filterByEditDistance(void *info, CAISS_SEARCH_TYPE searchType, HNSW_RET_TYPE &result,
                                        unsigned int filterEditDistance);
    CAISS_RET_TYPE filterByIgnoreTrie(HNSW_RET_TYPE &result);

    // 静态成员变量
private:
    static CAISS_RET_TYPE createHnswSingleton(SpaceInterface<CAISS_FLOAT> *distance_ptr, unsigned int maxDataSize, CAISS_BOOL normalize, unsigned int maxIndexSize=64,
                                              unsigned int maxNeighbor=32, unsigned int efSearch=100, unsigned int efConstruction=100);
    static CAISS_RET_TYPE createHnswSingleton(SpaceInterface<CAISS_FLOAT> *distance_ptr, const std::string &modelPath);
    static CAISS_RET_TYPE destroyHnswSingleton();
    static CAISS_RET_TYPE checkModelPrecisionEnable(float targetPrecision, unsigned int fastRank, unsigned int realRank,
                                                    const std::vector<CaissDataNode> &datas, float &calcPrecision);

    static HierarchicalNSW<CAISS_FLOAT>* getHnswSingleton();
    static CAISS_RET_TYPE insertByOverwrite(CAISS_FLOAT *node, unsigned int label, const char *index);
    static CAISS_RET_TYPE insertByDiscard(CAISS_FLOAT *node, unsigned int label, const char *index);

    static HierarchicalNSW<CAISS_FLOAT>*     hnsw_algo_ptr_;
    static RWLock                            hnsw_algo_lock_;

private:
    SpaceInterface<CAISS_FLOAT>*             distance_ptr_;    // 其实，这里也可以考虑用static了
    unsigned int                             neighbors_;
};


#endif //CHUNELCAISS_HNSWPROC_H
