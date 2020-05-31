//
// Created by Chunel on 2020/5/24.
//

#include "RapidJsonProc.h"

inline static std::string buildDistanceType(ANN_DISTANCE_TYPE type) {
    std::string ret;
    switch (type) {
        case ANN_DISTANCE_EUC:
            ret = "euclidean";
            break;
        case ANN_DISTANCE_INNER:
            ret = "cosine";
            break;
        case ANN_DISTANCE_EDITION:
            ret = "edition";
            break;
        default:
            break;
    }

    return ret;
}

RapidJsonProc::RapidJsonProc() {
}

RapidJsonProc::~RapidJsonProc() {
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


ANN_RET_TYPE RapidJsonProc::buildSearchResult(const std::vector<AnnResultDetail> &details,
        ANN_DISTANCE_TYPE distanceType, std::string &result) {
    ANN_FUNCTION_BEGIN

    Document dom;
    dom.SetObject();

    Document::AllocatorType& alloc = dom.GetAllocator();
    dom.AddMember("version", ANN_VERSION, alloc);
    dom.AddMember("size", details.size(), alloc);
    dom.AddMember("distance_type", StringRef(buildDistanceType(distanceType).c_str()), alloc);

    rapidjson::Value array(rapidjson::kArrayType);

    for (const AnnResultDetail& detail : details) {
        rapidjson::Value obj(rapidjson::kObjectType);
        obj.AddMember("distance", detail.distance, alloc);
        obj.AddMember("index", detail.index, alloc);    // 这里的index，表示的是这属于模型中的第几个节点

        rapidjson::Value node(rapidjson::kArrayType);
        for (auto j : detail.node) {
            node.PushBack(j, alloc);
        }
        obj.AddMember("node", node, alloc);
        array.PushBack(obj, alloc);
    }

    dom.AddMember("details", array, alloc);

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    dom.Accept(writer);
    result = buffer.GetString();    // 将最终的结果值，赋值给result信息，并返回

    ANN_FUNCTION_END
}


