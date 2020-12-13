//
// Created by Chunel on 2020/11/28.
//

#ifndef CAISS_ALGORITHMDEFINE_H
#define CAISS_ALGORITHMDEFINE_H

#include <string>
#if _USE_EIGEN3_
    #include <Eigen/Core>
    typedef Eigen::Map<Eigen::Array<float, 1, Eigen::Dynamic>> DynamicArrayType;
#endif

#include "./boost/bimap/bimap.hpp"
#include "../caissLib/CaissLib.h"

const static std::string CAISS_SEPARATOR = "|";
const static std::string QUERY_VIA_ARRAY = "";
const static std::string MODEL_SUFFIX = ".caiss";   // 默认的模型后缀

const static int HNSW_MODEL_TAG = 1001;
const static int NSG_MODEL_TAG = 1002;
const static int MRPT_MODEL_TAG = 1003;    // 模型标记，用于区分是否是mrpt模型

const static unsigned int DEFAULT_STEP = 2;
const static unsigned int DEFAULT_MAX_EPOCH = 5;
const static unsigned int DEFAULT_SHOW_SPAN = 1000;    // 1000行会显示一次日志

enum BIMAP_PROCESS_TYPE {
    BIMAP_INSERT = 0,
    BIMAP_FIND_INDEX = 1,    // 查询id信息，例：index=1
    BIMAP_FIND_LABEL = 2,    // 查询标签信息，例：label="hello"
    BIMAP_DELETE_INDEX = 3,
    BIMAP_DELETE_LABEL = 4
};


// 用于记录编号和词语的双向map
typedef boost::bimaps::bimap<unsigned int, std::string> BOOST_BIMAP;

inline static bool isAnnSuffix(const char *modelPath) {
    std::string path = std::string(modelPath);
    bool ret = (path.find(MODEL_SUFFIX) == path.length() - std::string(MODEL_SUFFIX).length());
    return ret;
}

inline static bool isWordSearchType(CAISS_SEARCH_TYPE searchType) {
    bool ret = false;
    if (CAISS_SEARCH_WORD == searchType
        || CAISS_LOOP_WORD == searchType) {
        ret = true;
    }
    return ret;
}

inline static bool isAnnSearchType(CAISS_SEARCH_TYPE searchType) {
    // 判定是否是快速查询类型
    bool ret = false;
    if (CAISS_SEARCH_WORD == searchType
        || CAISS_SEARCH_QUERY == searchType) {
        ret = true;
    }
    return ret;
}


#endif //CAISS_ALGORITHMDEFINE_H
