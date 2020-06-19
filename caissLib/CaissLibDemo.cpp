//
// Created by Chunel on 2020/6/14.
//

#include "CaissLib.h"
#include <iostream>
#include <string>


using namespace std;

static const unsigned int max_thread_num_ = 5;
static const CAISS_ALGO_TYPE algo_type_ = CAISS_ALGO_HNSW;
static const CAISS_MANAGE_TYPE manage_type_ = CAISS_MANAGE_SYNC;
static const CAISS_MODE mode_ = CAISS_MODE_PROCESS;
static const CAISS_DISTANCE_TYPE dist_type_ = CAISS_DISTANCE_INNER;
static const unsigned int dim_ = 768;
static const char *model_path_ = "../../doc/libDemo.caiss";
static const CAISS_DIST_FUNC dist_func_ = nullptr;
static std::string info_ = "decent";
static const CAISS_SEARCH_TYPE search_type_ = CAISS_SEARCH_WORD;
static const unsigned int top_k_ = 5;

static const string data_path_ = "../../doc/bert_71290words_768dim.txt";
static const unsigned int max_data_size_ = 100000;
static const CAISS_BOOL normalize_ = CAISS_TRUE;
static const unsigned int max_index_size_ = 64;
static const float precision_ = 0.95;
static const unsigned int fast_rank_ = 5;
static const unsigned int real_rank_ = 5;
static const unsigned int step_ = 1;
static const unsigned int max_epoch_ = 3;
static const unsigned int show_span_ = 1000;

static int train() {
    CAISS_FUNCTION_BEGIN

    void *handle = nullptr;
    ret = CAISS_CreateHandle(&handle);
    CAISS_FUNCTION_CHECK_STATUS

    ret = CAISS_Init(handle, CAISS_MODE_TRAIN, dist_type_, dim_, model_path_, dist_func_);
    CAISS_FUNCTION_CHECK_STATUS

    ret = CAISS_Train(handle, data_path_.c_str(), max_data_size_, normalize_, max_index_size_,
            precision_, fast_rank_, real_rank_, step_, max_epoch_, show_span_);
    CAISS_FUNCTION_CHECK_STATUS

    CAISS_FUNCTION_END
}

static int search() {
    CAISS_FUNCTION_BEGIN

    void *handle = nullptr;
    ret = CAISS_CreateHandle(&handle);
    CAISS_FUNCTION_CHECK_STATUS

    ret = CAISS_Init(handle, CAISS_MODE_PROCESS, dist_type_, dim_, model_path_, dist_func_);
    CAISS_FUNCTION_CHECK_STATUS

    ret = CAISS_Search(handle, (void *)info_.c_str(), search_type_, top_k_);
    CAISS_FUNCTION_CHECK_STATUS

    unsigned int size = 0;
    ret = CAISS_getResultSize(handle, size);
    CAISS_FUNCTION_CHECK_STATUS

    char *result = new char[size + 1];
    memset(result, 0, size + 1);
    ret = CAISS_getResult(handle, result, size);
    CAISS_FUNCTION_CHECK_STATUS
    std::cout << result << std::endl;
    delete [] result;

    ret = CAISS_destroyHandle(handle);
    CAISS_FUNCTION_CHECK_STATUS

    CAISS_FUNCTION_END
}

int main() {
    int ret = 0;
    ret = CAISS_Environment(max_thread_num_, algo_type_, manage_type_);
    CAISS_FUNCTION_CHECK_STATUS

    //ret = train();
    ret = search();
    return 0;
}

