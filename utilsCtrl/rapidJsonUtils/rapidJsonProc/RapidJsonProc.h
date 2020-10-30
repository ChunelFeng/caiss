//
// Created by Chunel on 2020/5/24.
//

#ifndef CAISS_RAPIDJSONPROC_H
#define CAISS_RAPIDJSONPROC_H

#include <list>
#include <string>

#include "../rapidjson/document.h"
#include "../../UtilsProc.h"
#include "../../UtilsInclude.h"
#include "../rapidjson/stringbuffer.h"
#include "../rapidjson/writer.h"


using namespace rapidjson;

class RapidJsonProc : public UtilsProc  {
public:
    explicit RapidJsonProc();
    ~RapidJsonProc();

    static CAISS_STATUS parseInputData(const char *line, CaissDataNode& node);
    static CAISS_STATUS buildSearchResult(const ALOG_WORD2DETAILS_MAP &word2DetailsMap,
                                          CAISS_DISTANCE_TYPE distanceType,
                                          const std::string& searchType,
                                          const unsigned int topK,
                                          std::string &result);
};


#endif //CAISS_RAPIDJSONPROC_H
