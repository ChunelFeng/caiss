/***************************
@Author: Chunel
@Contact: chunel@foxmail.com
@File: MrptProc.h
@Time: 2020/12/4 7:42 下午
@Desc: 改编自：https://github.com/vioshyvo/mrpt
***************************/

#ifndef CAISS_MRPTPROC_H
#define CAISS_MRPTPROC_H

#include <vector>
#include <string>

#include "../../AlgorithmProc.h"
#include "../mrptAlgo/Mrpt.h"
#include "MrptProcDefine.h"

enum BIMAP_PROCESS_TYPE {
    BIMAP_INSERT = 0,
    BIMAP_FIND_INDEX = 1,    // 查询id信息，例：index=1
    BIMAP_FIND_LABEL = 2,    // 查询标签信息，例：label="hello"
    BIMAP_DELETE_INDEX = 3,
    BIMAP_DELETE_LABEL = 4
};


class MrptProc : public AlgorithmProc {
public:
    explicit MrptProc();
    ~MrptProc() override;

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
    CAISS_STATUS ignore(const char *label, CAISS_BOOL isIgnore) override;

protected:
    CAISS_STATUS reset();
    CAISS_STATUS loadDatas(const char *dataPath);
    CAISS_STATUS innerSearchResult(void *info,
                                   CAISS_SEARCH_TYPE searchType,
                                   unsigned int topK,
                                   unsigned int filterEditDistance);
    CAISS_STATUS loadModel();
    CAISS_STATUS loadModelHead();

    CAISS_STATUS buildResult(unsigned int topK, CAISS_SEARCH_TYPE searchType,
                             const ALOG_WORD2RESULT_MAP &word2ResultMap);


protected:
    // 处理label-index对应的map
    static CAISS_STATUS processIndexLabel(BIMAP_PROCESS_TYPE type,
                                          unsigned int &index,
                                          std::string &label);

    static CAISS_STATUS getAllLabels(std::vector<std::string>& labels);

private:
    // 算法模型相关内容
    static Singleton<Mrpt> mrpt_algo_;
    static Singleton<MrptModelHead> mrpt_model_head_;    // 算法头文件

    static RWLock bimap_lock_;
    static BOOST_BIMAP index_label_lookup_;    // left是unsigned int，right是string
};


#endif //CAISS_MRPTPROC_H
