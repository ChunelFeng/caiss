//
// Created by Chunel on 2020/8/30.
//

#include "CaissSql.h"


int demo_sql_search(){

    CAISS_ECHO("enter sql search demo...");
    CAISS_FUNCTION_BEGIN

    void *handle = nullptr;
    ret = CAISS_CreateHandle(&handle);
    CAISS_FUNCTION_CHECK_STATUS

    ret = CAISS_Init(handle, CAISS_MODE_PROCESS, dist_type_, dim_, model_path_, dist_func_);
    CAISS_FUNCTION_CHECK_STATUS

    string sql = "select * from bert_71290words_768dim where word like water limit 8";
    ret = CAISS_ExecuteSQL(handle, sql.c_str());
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
