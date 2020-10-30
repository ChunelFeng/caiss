//
// Created by Chunel on 2020/6/20.
//

#include "../CaissDemoInclude.h"


int demo_train() {
    CAISS_ECHO("enter train demo...");
    CAISS_FUNCTION_BEGIN

    void *handle = nullptr;
    ret = CAISS_CreateHandle(&handle);
    CAISS_FUNCTION_CHECK_STATUS

    ret = CAISS_Init(handle, CAISS_MODE_TRAIN, dist_type_, dim_, model_path_, dist_func_);
    CAISS_FUNCTION_CHECK_STATUS

    ret = CAISS_Train(handle, data_path_, max_data_size_, normalize_, max_index_size_,
                              precision_, fast_rank_, real_rank_, step_, max_epoch_, show_span_);
    CAISS_FUNCTION_CHECK_STATUS

    ret = CAISS_DestroyHandle(handle);
    CAISS_FUNCTION_CHECK_STATUS

    CAISS_FUNCTION_END
}

int demo_search() {
    CAISS_ECHO("enter search demo...");
    CAISS_FUNCTION_BEGIN

    void *handle = nullptr;
    ret = CAISS_CreateHandle(&handle);
    CAISS_FUNCTION_CHECK_STATUS

    ret = CAISS_Init(handle, CAISS_MODE_PROCESS, dist_type_, dim_, model_path_, dist_func_);
    CAISS_FUNCTION_CHECK_STATUS

    ret = CAISS_Search(handle, (void *)info_, search_type_, top_k_, filter_edit_distance_);
    CAISS_FUNCTION_CHECK_STATUS

    unsigned int size = 0;
    ret = CAISS_GetResultSize(handle, size);
    CAISS_FUNCTION_CHECK_STATUS

    char *result = new char[size + 1];
    memset(result, 0, size + 1);
    ret = CAISS_GetResult(handle, result, size);
    CAISS_FUNCTION_CHECK_STATUS
    std::cout << result << std::endl;
    delete [] result;

    ret = CAISS_DestroyHandle(handle);
    CAISS_FUNCTION_CHECK_STATUS

    CAISS_FUNCTION_END
}

int demo_insert() {
    CAISS_ECHO("enter insert demo...");
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

    string word = "caiss";    // 生成一个词语
    ret = CAISS_Insert(handle, vec.data(),  word.c_str(), CAISS_INSERT_OVERWRITE);
    CAISS_FUNCTION_CHECK_STATUS

    // 插入后，再次查询，会查到新插入的词语
    // 是否将新插入的信息，记录在模型中，取决于是否调用CAISS_Save函数
    //ret = CAISS_Save(handle);
    //CAISS_FUNCTION_CHECK_STATUS

    ret = CAISS_Search(handle, (void *)word.c_str(), search_type_, top_k_);
    CAISS_FUNCTION_CHECK_STATUS

    unsigned int size = 0;
    ret = CAISS_GetResultSize(handle, size);
    CAISS_FUNCTION_CHECK_STATUS

    char *result = new char[size + 1];
    memset(result, 0, size + 1);
    ret = CAISS_GetResult(handle, result, size);
    CAISS_FUNCTION_CHECK_STATUS
    std::cout << result << std::endl;
    delete [] result;

    ret = CAISS_DestroyHandle(handle);
    CAISS_FUNCTION_CHECK_STATUS

    CAISS_FUNCTION_END
}

int demo_ignore() {
    CAISS_ECHO("enter ignore demo...");
    CAISS_FUNCTION_BEGIN

    void *handle = nullptr;
    ret = CAISS_CreateHandle(&handle);
    CAISS_FUNCTION_CHECK_STATUS

    ret = CAISS_Init(handle, CAISS_MODE_PROCESS, dist_type_, dim_, model_path_, dist_func_);
    CAISS_FUNCTION_CHECK_STATUS

    // 忽略一些关键词（针对water来说，wine和mud属于关键词）
    string ignoreWord1 = "wine";
    string ignoreWord2 = "mud";
    ret = CAISS_Ignore(handle, ignoreWord1.c_str());    // isIgnore参数的默认值是true，表示忽略
    CAISS_FUNCTION_CHECK_STATUS
    ret = CAISS_Ignore(handle, ignoreWord2.c_str(), CAISS_TRUE);
    CAISS_FUNCTION_CHECK_STATUS

    ret = CAISS_Search(handle, (void *)info_, search_type_, top_k_, filter_edit_distance_);
    CAISS_FUNCTION_CHECK_STATUS

    unsigned int size = 0;
    ret = CAISS_GetResultSize(handle, size);
    CAISS_FUNCTION_CHECK_STATUS

    char *result = new char[size + 1];
    memset(result, 0, size + 1);
    ret = CAISS_GetResult(handle, result, size);
    CAISS_FUNCTION_CHECK_STATUS
    std::cout << result << std::endl;    // ignore的时候展示结果

    // 是否保存ignore中的数据，取决于是否调用save函数信息
    // ret = CAISS_Save(handle);
    // CAISS_FUNCTION_CHECK_STATUS

    // 将之前忽略的部分关键词添加回来，重新查找
    ret = CAISS_Ignore(handle, ignoreWord1.c_str(), CAISS_FALSE);
    CAISS_FUNCTION_CHECK_STATUS

    ret = CAISS_Search(handle, (void *)info_, search_type_, top_k_, filter_edit_distance_);
    CAISS_FUNCTION_CHECK_STATUS

    size = 0;
    ret = CAISS_GetResultSize(handle, size);
    CAISS_FUNCTION_CHECK_STATUS

    memset(result, 0, size + 1);
    ret = CAISS_GetResult(handle, result, size);
    CAISS_FUNCTION_CHECK_STATUS
    std::cout << result << std::endl;    // 不再ignore之后，再次展示结果
    delete [] result;

    ret = CAISS_DestroyHandle(handle);
    CAISS_FUNCTION_CHECK_STATUS

    CAISS_FUNCTION_END
}
