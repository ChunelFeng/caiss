//
// Created by Chunel on 2020/6/20.
//


#include "../CaissDemoInclude.h"



int demo_train() {
    CAISS_FUNCTION_BEGIN

    void *handle = nullptr;
    ret = CAISS_CreateHandle(&handle);
    CAISS_FUNCTION_CHECK_STATUS

    ret = CAISS_Init(handle, CAISS_MODE_TRAIN, dist_type_, dim_, model_path_, dist_func_);
    CAISS_FUNCTION_CHECK_STATUS

    ret = CAISS_Train(handle, data_path_, max_data_size_, normalize_, max_index_size_,
                              precision_, fast_rank_, real_rank_, step_, max_epoch_, show_span_);
    CAISS_FUNCTION_CHECK_STATUS

    ret = CAISS_destroyHandle(handle);
    CAISS_FUNCTION_CHECK_STATUS

    CAISS_FUNCTION_END
}

int demo_search() {
    CAISS_FUNCTION_BEGIN

    void *handle = nullptr;
    ret = CAISS_CreateHandle(&handle);
    CAISS_FUNCTION_CHECK_STATUS

    ret = CAISS_Init(handle, CAISS_MODE_PROCESS, dist_type_, dim_, model_path_, dist_func_);
    CAISS_FUNCTION_CHECK_STATUS

    ret = CAISS_Search(handle, (void *)info_, search_type_, top_k_);
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

int demo_insert() {
    CAISS_FUNCTION_BEGIN

    void *handle = nullptr;
    ret = CAISS_CreateHandle(&handle);
    CAISS_FUNCTION_CHECK_STATUS

    ret = CAISS_Init(handle, CAISS_MODE_PROCESS, dist_type_, dim_, model_path_, dist_func_);
    CAISS_FUNCTION_CHECK_STATUS

    vector<float> vec;    // 演示insert功能，随机生成的向量
    for (int i = 0; i < dim_; i++) {
        vec.push_back(1.0f);
    }

    string word = "this-is-a-new-word";    // 生成一个词语
    ret = CAISS_Insert(handle, vec.data(),  word.c_str(), CAISS_INSERT_OVERWRITE);
    CAISS_FUNCTION_CHECK_STATUS

    // 插入后，再次查询，会查到新插入的词语
    // 是否将新插入的信息，记录在模型中，取决于是否调用CAISS_Save函数
    //ret = CAISS_Save(handle);
    //CAISS_FUNCTION_CHECK_STATUS

    ret = CAISS_Search(handle, (void *)word.c_str(), search_type_, top_k_);
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