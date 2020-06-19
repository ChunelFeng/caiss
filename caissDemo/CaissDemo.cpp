//
// Created by Chunel on 2020/6/14.
//


#include "CaissDemoInclude.h"


int main() {
    int ret = 0;
    ret = CAISS_Environment(max_thread_num_, algo_type_, manage_type_);
    CAISS_FUNCTION_CHECK_STATUS

    //ret = train();
    //ret = search();
    return 0;
}

