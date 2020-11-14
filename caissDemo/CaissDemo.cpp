//
// Created by Chunel on 2020/6/14.
// demo入口
//

#include "CaissDemoInclude.h"


int CaissDemo() {
    CAISS_STATUS ret = CAISS_RET_OK;
    // 无论何种使用模式，使用caiss前，请调用环境函数
    ret = CAISS_Environment(max_thread_num_, algo_type_, manage_type_);
    CAISS_FUNCTION_CHECK_STATUS

    ret = demo_train();
    CAISS_FUNCTION_CHECK_STATUS

    ret = demo_search();
    //ret = demo_insert();
    //ret = demo_ignore();
    //ret = demo_asyncMultiThreadSearch();
    //ret = demo_syncMultiThreadSearch();
    //ret = demo_sql_search();
    CAISS_FUNCTION_CHECK_STATUS

    return ret;
}

int main() {
    CaissDemo();

    return 0;
}

