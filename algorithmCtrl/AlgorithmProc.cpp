/***************************
@Author: Chunel
@Contact: chunel@foxmail.com
@File: AlgorithmProc.cpp
@Time: 2020/12/12 4:51 下午
@Desc: 
***************************/

#include "AlgorithmProc.h"


/**
 * 获取结果的长度
 * @param size
 * @return
 */
CAISS_STATUS AlgorithmProc::getResultSize(unsigned int& size) {
    CAISS_FUNCTION_BEGIN
    CAISS_CHECK_MODE_ENABLE(CAISS_MODE_PROCESS)

    if (this->result_.empty()) {
        ret = CAISS_RET_RESULT_SIZE;
    }

    CAISS_FUNCTION_CHECK_STATUS

    size = this->result_.size();

    CAISS_FUNCTION_END
}


/**
 * 获取结果
 * @param result
 * @param size
 * @return
 */
CAISS_STATUS AlgorithmProc::getResult(char *result, unsigned int size) {
    CAISS_FUNCTION_BEGIN
    CAISS_ASSERT_NOT_NULL(result)
    CAISS_CHECK_MODE_ENABLE(CAISS_MODE_PROCESS)

    if (result_.size() > size) {
        ret = CAISS_RET_RESULT_SIZE;    // 如果分配的尺寸小了，则返回异常
    }

    CAISS_FUNCTION_CHECK_STATUS

    memset(result, 0, size);
    memcpy(result, this->result_.data(), this->result_.size());

    CAISS_FUNCTION_END
}


/**
 * 将某个节点放入忽略列表中，或者从忽略列表中取消
 * @param label
 * @param isIgnore 放入忽略列表/从忽略列表中取出
 * @return
 */
CAISS_STATUS AlgorithmProc::ignore(const char *label,
                                   CAISS_BOOL isIgnore) {
    CAISS_FUNCTION_BEGIN
    CAISS_ASSERT_NOT_NULL(label)
    CAISS_CHECK_MODE_ENABLE(CAISS_MODE_PROCESS)    // process 模式下，才能进行

    const string& info = std::string(label);
    if (isIgnore) {
        // 对于外部的 ignore 当前单词，相当于是在字典树中，加入这个词语
        AlgorithmProc::getIgnoreTrie()->insert(info);
    } else {
        // 对于外部的 not-ignore，相当于是在字典树中
        AlgorithmProc::getIgnoreTrie()->eraser(info);
    }

    this->last_topK_ = 0;    // 如果插入成功，则重新记录topK信息
    this->last_search_type_ = CAISS_SEARCH_DEFAULT;

    CAISS_FUNCTION_END
}


CAISS_STATUS AlgorithmProc::processCallBack(CAISS_SEARCH_CALLBACK searchCBFunc,
                                            const void *cbParams) {
    CAISS_FUNCTION_BEGIN

    if (nullptr != searchCBFunc) {
        for (const auto& node : word_details_map_) {
            const char* query = node.first.c_str();    // 记录查询单词信息
            CAISS_STRING_ARRAY infos;
            CAISS_FLOAT_ARRAY distances;
            for (const CaissResultDetail &detail : node.second) {
                infos.push_back(detail.label);
                distances.push_back(detail.distance);
            }

            // 每条信息通过一次处理函数
            searchCBFunc(query, infos, distances, cbParams);
        }
    }

    CAISS_FUNCTION_END
}


CAISS_STATUS AlgorithmProc::normalizeNode(std::vector<CAISS_FLOAT>& node, unsigned int dim) {
    if (CAISS_FALSE == this->normalize_) {
        return CAISS_RET_OK;    // 如果不需要归一化，直接返回
    }

    if (dim != this->dim_) {
        return CAISS_RET_DIM;    // 忽略维度不一致的情况
    }

#ifdef _USE_EIGEN3_
    // 如果有找到eigen的情况下，加速计算（只计算前dim个信息）
    DynamicArrayType arr(node.data(), (const int)this->dim_);
    auto denominator = 1 / sqrt((arr * arr).sum());
    arr = arr * denominator;    // arr 就是归一化之后的矩阵信息
    node.assign(arr.data(), arr.data() + this->dim_);
#else
    CAISS_FLOAT sum = 0.0;
        for (unsigned int i = 0; i < this->dim_; i++) {
            sum += (node[i] * node[i]);
        }

        CAISS_FLOAT denominator = 1 / (float)std::sqrt(sum);    // 分母信息
        for (unsigned int i = 0; i < this->dim_; i++) {
            node[i] = node[i] * denominator;
        }
#endif
    return CAISS_RET_OK;
}


CAISS_STATUS AlgorithmProc::filterByRules(void *info, const CAISS_SEARCH_TYPE searchType,
                                          ALGO_RET_TYPE &result, unsigned int topK,
                                          const unsigned int filterEditDistance,
                                          const BOOST_BIMAP &indexLabelLookup) {
    CAISS_FUNCTION_BEGIN

    // 今后可能有多种规则
    ret = filterByEditDistance(info, searchType, result, filterEditDistance, indexLabelLookup);
    CAISS_FUNCTION_CHECK_STATUS

    // 过滤掉被删除的信息
    ret = filterByIgnoreTrie(result, indexLabelLookup);
    CAISS_FUNCTION_CHECK_STATUS

    // 所有的情况都过滤完了之后，保证不会超过topK个
    while (result.size() > topK) {
        result.pop();
    }

    CAISS_FUNCTION_END
}


CAISS_STATUS AlgorithmProc::filterByEditDistance(void *info,
                                                 CAISS_SEARCH_TYPE searchType,
                                                 ALGO_RET_TYPE &result,
                                                 unsigned int filterEditDistance,
                                                 const BOOST_BIMAP &indexLabelLookup) {
    CAISS_FUNCTION_BEGIN
    CAISS_ASSERT_NOT_NULL(info)

    if (!isWordSearchType(searchType)    // 如果不是根据word查询，则不需要走这一步
        || (CAISS_MIN_EDIT_DISTANCE == filterEditDistance)) {   // 值=-1，不需要根据编辑距离来过滤
        return CAISS_RET_OK;
    }

    if (CAISS_MAX_EDIT_DISTANCE < filterEditDistance) {
        return CAISS_RET_PARAM;    // 如果值设置的太大了，则返回参数校验错误
    }

    string word = std::string((char *)info);    // 已经确定是查词语类型的了
    ALGO_RET_TYPE resultBackUp;

    while (!result.empty()) {
        auto cur = result.top();
        result.pop();
        string candWord = indexLabelLookup.left.find(cur.second)->second;    // 这里的label，是单词信息
        if (EditDistanceProc::BeyondEditDistance(candWord, word, filterEditDistance)) {
            resultBackUp.push(cur);    // 仅添加超过范围的
        }
    }

    result = resultBackUp;
    CAISS_FUNCTION_END
}


/**
 * 通过忽略trie来做过滤
 * @param info
 * @param searchType
 * @param result
 * @return
 */
CAISS_STATUS AlgorithmProc::filterByIgnoreTrie(ALGO_RET_TYPE &result,
                                               const BOOST_BIMAP &indexLabelLookup) {
    CAISS_FUNCTION_BEGIN
    CAISS_ASSERT_NOT_NULL(AlgorithmProc::getIgnoreTrie())

    ALGO_RET_TYPE resultBackUp;

    while (!result.empty()) {
        auto cur = result.top();
        result.pop();
        string candWord = indexLabelLookup.left.find(cur.second)->second;
        if (!AlgorithmProc::getIgnoreTrie()->find(candWord)) {
            resultBackUp.push(cur);    // 如果这些词语，不在过滤trie树上，就添加进来
        }
    }

    result = resultBackUp;

    CAISS_FUNCTION_END
}