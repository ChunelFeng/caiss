//
// Created by Chunel on 2020/5/24.
//

#ifndef CHUNELANN_RAPIDJSONPROC_H
#define CHUNELANN_RAPIDJSONPROC_H

#include <vector>


#include "../rapidjson/document.h"
#include "../../UtilsProc.h"
#include "../../UtilsInclude.h"
#include "../rapidjson/stringbuffer.h"
#include "../rapidjson/writer.h"

struct AnnResultDetail {
    std::vector<ANN_FLOAT> node;
    ANN_FLOAT distance;
    unsigned int index;
};

using namespace rapidjson;

class RapidJsonProc : public UtilsProc  {
public:
    RapidJsonProc();
    ANN_RET_TYPE init() override ;
    ANN_RET_TYPE deinit() override ;

    ANN_RET_TYPE parseInputData(const char *data, std::vector<ANN_FLOAT>& node);
    ANN_RET_TYPE buildSearchResult(const std::vector<AnnResultDetail> &details, std::string &result);

protected:
    ~RapidJsonProc();
};


#endif //CHUNELANN_RAPIDJSONPROC_H
