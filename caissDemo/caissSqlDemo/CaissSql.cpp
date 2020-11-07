//
// Created by Chunel on 2020/8/30.
// sql版本demo
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

    vector<float> vec1;    // 插入sql语句中，需要插入的向量
    vector<float> vec2;
    for (int i = 0; i < dim_; i++) {
        vec1.push_back((float)(i%2));    // 随机生成两个
        vec2.push_back((float)(i%3));
    }

    auto value1 = std::to_string((int64_t)vec1.data());    // 传入的是对应信息的首地址的字符串形式
    auto value2 = std::to_string((int64_t)vec2.data());

    string sql = "SELECT * FROM bert_71290words_768dim WHERE word LIKE 'water' limit 5";    // 查询语句
    //string sql = "INSERT INTO bert_71290words_768dim(key1, key2) VALUES(" + value1 + "," + value2 + ")";    // 插入语句（插入key1，key2两个词语及其对应的向量信息）
    //string sql = "DELETE FROM bert_71290words_768dim WHERE word = 'water'";    // 删除语句
    //string sql = "UPDATE bert_71290words_768dim SET tensor=" + value1 + " WHERE word='water'";     // 更新语句。如果模型中无信息，则直接插入
    ret = CAISS_ExecuteSQL(handle, sql.c_str());
    CAISS_FUNCTION_CHECK_STATUS

    unsigned int size = 0;
    ret = CAISS_GetResultSize(handle, size);
    CAISS_FUNCTION_CHECK_STATUS

    char *result = new char[size + 1];
    memset(result, 0, size + 1);
    ret = CAISS_GetResult(handle, result, size);
    CAISS_FUNCTION_CHECK_STATUS
    std::cout << result << std::endl;    // 执行查询的sql语句的时候，会打印结果信息
    delete[] result;

    ret = CAISS_DestroyHandle(handle);
    CAISS_FUNCTION_CHECK_STATUS

    CAISS_FUNCTION_END
}
