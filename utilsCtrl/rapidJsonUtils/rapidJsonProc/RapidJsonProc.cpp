//
// Created by Chunel on 2020/5/24.
//

#include "RapidJsonProc.h"

RapidJsonProc::RapidJsonProc() {
    return;
}

RapidJsonProc::~RapidJsonProc() {
    return;
}


ANN_RET_TYPE RapidJsonProc::init() {
    ANN_FUNCTION_END
}

ANN_RET_TYPE RapidJsonProc::deinit() {
    ANN_FUNCTION_END
}


ANN_RET_TYPE RapidJsonProc::parseInputData(const char *data, std::vector<ANN_FLOAT>& node) {
    ANN_ASSERT_NOT_NULL(data)

    ANN_FUNCTION_BEGIN

    node.clear();    // 首先清空node中的内容
    Document dom;
    dom.Parse(data);    // data是一行数据

    if (dom.HasParseError()) {
        return ANN_RET_JSON;
    }

    Value& jsonArray = dom;
    if (!jsonArray.IsArray()) {
        return ANN_RET_JSON;
    }

    unsigned int size = jsonArray.Size();
    for (unsigned int i = 0; i < size; ++i) {
        node.push_back(jsonArray[i].GetFloat());   // 给node赋值
    }

    dom.Clear();
    ANN_FUNCTION_END
}


