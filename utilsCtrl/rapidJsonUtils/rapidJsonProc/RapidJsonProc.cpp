//
// Created by Chunel on 2020/5/24.
//

#include "RapidJsonProc.h"
#include <string>


inline static std::string buildDistanceType(CAISS_DISTANCE_TYPE type) {
    std::string ret;
    switch (type) {
        case CAISS_DISTANCE_EUC:
            ret = "euclidean";
            break;
        case CAISS_DISTANCE_INNER:
            ret = "inner";
            break;
        case CAISS_DISTANCE_JACCARD:
            ret = "jaccard";
            break;
        case CAISS_DISTANCE_EDITION:
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


CAISS_RET_TYPE RapidJsonProc::parseInputData(const char *line, CaissDataNode& dataNode) {
    CAISS_ASSERT_NOT_NULL(line)

    CAISS_FUNCTION_BEGIN

    Document dom;
    dom.Parse(line);    // data是一行数据，形如：{"hello" : ["1","0","0","0"]}

    if (dom.HasParseError()) {
        return CAISS_RET_JSON;
    }

    Value& jsonObject = dom;
    if (!jsonObject.IsObject()) {
        return CAISS_RET_JSON;
    }

    for (Value::ConstMemberIterator itr = jsonObject.MemberBegin(); itr != jsonObject.MemberEnd(); ++itr) {
        dataNode.index = itr->name.GetString();    // 获取行名称
        rapidjson::Value& array = jsonObject[dataNode.index.c_str()];
        for (unsigned int i = 0; i < array.Size(); ++i) {
            dataNode.node.push_back((CAISS_FLOAT)strtod(array[i].GetString(), nullptr));
        }
    }

    CAISS_FUNCTION_END
}


CAISS_RET_TYPE
RapidJsonProc::buildSearchResult(const std::list<CaissResultDetail> &details, CAISS_DISTANCE_TYPE distanceType,
                                 std::string &result, std::string searchType) {
    CAISS_FUNCTION_BEGIN

    Document dom;
    dom.SetObject();

    Document::AllocatorType& alloc = dom.GetAllocator();
    dom.AddMember("version", CAISS_VERSION, alloc);
    dom.AddMember("size", StringRef(std::to_string((int)details.size()).c_str()), alloc);

    std::string distType = buildDistanceType(distanceType);    // 需要在这里开一个string，然后再构建json。否则release版本无法使用
    dom.AddMember("distance_type", StringRef(distType.c_str()), alloc);
    dom.AddMember("search_type", StringRef(searchType.c_str()), alloc);

    rapidjson::Value array(rapidjson::kArrayType);

    for (const CaissResultDetail& detail : details) {
        rapidjson::Value obj(rapidjson::kObjectType);

        float distance = (detail.distance < 0.00001 && detail.distance > -0.00001) ? (0.0f) : detail.distance;
        obj.AddMember("distance", StringRef(std::to_string(distance).c_str()), alloc);
        obj.AddMember("index", StringRef(std::to_string(detail.index).c_str()), alloc);    // 这里的index，表示的是这属于模型中的第几个节点(注：跟算法类中，index和label的取名正好相反)
        obj.AddMember("label", StringRef(detail.label.c_str()), alloc);    // 这里的label，表示单词信息
//        rapidjson::Value node(rapidjson::kArrayType);    // 输出向量的具体内容，暂时不需要了
//        for (auto j : detail.node) {
//            node.PushBack(j, alloc);
//        }
//        obj.AddMember("node", node, alloc);
        array.PushBack(obj, alloc);
    }

    dom.AddMember("details", array, alloc);

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    dom.Accept(writer);
    result = buffer.GetString();    // 将最终的结果值，赋值给result信息，并返回

    CAISS_FUNCTION_END
}


CAISS_RET_TYPE
RapidJsonProc::parseResult(const std::string &result, CAISS_LIST_STRING &resultWords, CAISS_LIST_FLOAT &resultDistances) {
    CAISS_FUNCTION_BEGIN

    rapidjson::Document dom;
    dom.Parse(result.c_str());

    if (dom.HasParseError()) {
        return CAISS_RET_JSON;
    }

    Value& jsonObject = dom;
    if (!jsonObject.IsObject()) {
        return CAISS_RET_JSON;
    }

    if (dom.HasMember("details")) {
        rapidjson::Value& detailsValue = dom["details"];
        for(rapidjson::SizeType i = 0; i < detailsValue.Size(); ++i){
            const rapidjson::Value& dv = detailsValue[i];
            resultDistances.push_back(dv["distance"].GetFloat());
            resultWords.push_back(dv["label"].GetString());
        }
    } else {
        return CAISS_RET_JSON;
    }

    CAISS_FUNCTION_END
}

