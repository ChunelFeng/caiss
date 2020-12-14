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


CAISS_STATUS RapidJsonProc::parseInputData(const char *line, CaissDataNode& dataNode) {
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
        dataNode.label = itr->name.GetString();    // 获取行名称
        rapidjson::Value& array = jsonObject[dataNode.label.c_str()];
        for (unsigned int i = 0; i < array.Size(); ++i) {
            dataNode.node.push_back((CAISS_FLOAT)strtod(array[i].GetString(), nullptr));
        }
    }

    CAISS_FUNCTION_END
}


CAISS_STATUS
RapidJsonProc::buildSearchResult(const ALOG_WORD2DETAILS_MAP &word2DetailsMap, CAISS_DISTANCE_TYPE distanceType,
                                 const std::string &searchType, unsigned int topK,
                                 AlgoTimerProc *timerProc,
                                 std::string &result) {
    CAISS_FUNCTION_BEGIN

    CAISS_ASSERT_NOT_NULL(timerProc)

    Document dom;
    dom.SetObject();

    Document::AllocatorType& alloc = dom.GetAllocator();
    dom.AddMember("version", CAISS_VERSION, alloc);
    Value val(Type::kObjectType);
    val.SetInt((int)topK);
    dom.AddMember("size", val, alloc);

    // 需要在这里开一个string，然后再构建json。否则release版本无法使用
    std::string distType = buildDistanceType(distanceType);
    dom.AddMember("distance_type", StringRef(distType.c_str()), alloc);
    dom.AddMember("search_type", StringRef(searchType.c_str()), alloc);

    // 字段取名ann_ts，因为还有lru或者loop的情况
    dom.AddMember("ann_ts", timerProc->getAlgoTimeCost(), alloc);
    dom.AddMember("search_ts", timerProc->getFuncTimeCost(), alloc);
    dom.AddMember("algo_type", StringRef(timerProc->getAlgoType()), alloc);

    rapidjson::Value detailsArray(rapidjson::kArrayType);
    for (const auto& x : word2DetailsMap) {
        Value info(rapidjson::kObjectType);
        info.AddMember("query", StringRef(x.first.c_str()), alloc);

        rapidjson::Value array(rapidjson::kArrayType);
        auto details = x.second;
        for (const CaissResultDetail& detail : details) {
            rapidjson::Value obj(rapidjson::kObjectType);
            val.SetFloat((detail.distance < 0.00001 && detail.distance > -0.00001) ? (0.0f) : detail.distance);
            obj.AddMember("distance", val, alloc);
            val.SetInt((int)detail.index);
            obj.AddMember("index", val, alloc);    // 这里的index，表示的是这属于模型中的第几个节点(注：跟算法类中，index和label的取名正好相反)

            val.SetString(StringRef(detail.label.c_str()), detail.label.size(), alloc);
            obj.AddMember("label", val, alloc);    // 这里的label，表示单词信息
            // rapidjson::Value node(rapidjson::kArrayType);    // 打印向量信息
            // for (auto j : detail.node) {
            //     node.PushBack(j, alloc);
            // }
            // obj.AddMember("node", node, alloc);
            array.PushBack(obj, alloc);
        }

        info.AddMember("details", array, alloc);
        detailsArray.PushBack(info, alloc);
    }

    dom.AddMember("result", detailsArray, alloc);

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    dom.Accept(writer);
    result = buffer.GetString();    // 将最终的结果值，赋值给result信息，并返回

    CAISS_FUNCTION_END
}

