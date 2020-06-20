//
// Created by Chunel on 2020/6/14.
//


#include "CaissDemoInclude.h"


int main() {
    int ret = 0;
    // 无论何种使用情况，使用caiss前，请调用环境函数
    ret = CAISS_Environment(max_thread_num_, algo_type_, manage_type_);
    CAISS_FUNCTION_CHECK_STATUS

    //ret = train();
    ret = demo_search();
    //ret = insert();
    return 0;
}

