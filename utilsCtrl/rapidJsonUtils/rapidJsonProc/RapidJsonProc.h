//
// Created by Chunel on 2020/5/24.
//

#ifndef CHUNELANN_RAPIDJSONPROC_H
#define CHUNELANN_RAPIDJSONPROC_H

#include "../rapidjson/document.h"
#include "../../UtilsProc.h"
#include "../../UtilsInclude.h"

using namespace rapidjson;

class RapidJsonProc : public UtilsProc  {
public:
    RapidJsonProc();
    ANN_RET_TYPE init() override ;
    ANN_RET_TYPE deinit() override ;

    ANN_RET_TYPE parseInputData(const char *data, std::vector<ANN_FLOAT>& node);

protected:
    ~RapidJsonProc() override;
};


#endif //CHUNELANN_RAPIDJSONPROC_H
