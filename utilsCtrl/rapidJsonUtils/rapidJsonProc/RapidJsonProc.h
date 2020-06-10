//
// Created by Chunel on 2020/5/24.
//

#ifndef CHUNELCAISS_RAPIDJSONPROC_H
#define CHUNELCAISS_RAPIDJSONPROC_H

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
    CAISS_RET_TYPE init() override ;
    CAISS_RET_TYPE deinit() override ;

    static CAISS_RET_TYPE parseInputData(const char *line, AnnDataNode& node);
    static CAISS_RET_TYPE buildSearchResult(const std::list<AnnResultDetail> &details, CAISS_DISTANCE_TYPE distanceType, std::string &result);
};


#endif //CHUNELCAISS_RAPIDJSONPROC_H
