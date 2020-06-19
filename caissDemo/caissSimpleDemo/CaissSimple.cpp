//
// Created by Chunel on 2020/6/20.
//

#include "../CaissDemoInclude.h"
#include <time.h>


static clock_t start_time_;
static clock_t end_time_;

int train() {
    CAISS_FUNCTION_BEGIN

    void *handle = nullptr;
    ret = CAISS_CreateHandle(&handle);
    CAISS_FUNCTION_CHECK_STATUS

    ret = CAISS_Init(handle, CAISS_MODE_TRAIN, dist_type_, dim_, model_path_, dist_func_);
    CAISS_FUNCTION_CHECK_STATUS

    ret = CAISS_Train(handle, data_path_, max_data_size_, normalize_, max_index_size_,
                              precision_, fast_rank_, real_rank_, step_, max_epoch_, show_span_);
    CAISS_FUNCTION_CHECK_STATUS

    CAISS_FUNCTION_END
}

int search() {
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
    //std::cout << result << std::endl;
    delete [] result;

    ret = CAISS_destroyHandle(handle);
    CAISS_FUNCTION_CHECK_STATUS

    CAISS_FUNCTION_END
}
