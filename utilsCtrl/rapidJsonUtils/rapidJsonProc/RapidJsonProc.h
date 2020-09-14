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

    static CAISS_RET_TYPE parseInputData(const char *line, CaissDataNode& node);
    static CAISS_RET_TYPE buildSearchResult(const std::list<CaissResultDetail> &details, CAISS_DISTANCE_TYPE distanceType,
                                            std::string &result, const std::string& searchType);
    static CAISS_RET_TYPE parseResult(const std::string& result, CAISS_LIST_STRING& resultWords, CAISS_LIST_FLOAT& resultDistances);
};


#endif //CAISS_RAPIDJSONPROC_H
