//
// Created by Chunel on 2020/6/20.
// 包含同步调用和异步调用的多线程demo
//

#include <thread>
#include <functional>
#include <future>
#include "../CaissDemoInclude.h"

const static vector<string> WORDS = {"this", "is", "an", "open", "source", "project", "and", "hope", "it", "will", "be", "useful", "for", "you", "best", "wishes"};
const static int SEARCH_TIMES = 100;

void STDCALL searchCallbackFunc(CAISS_LIST_STRING& words, CAISS_LIST_FLOAT& distances, const void *params) {
    cout << "query word is : " << (char *)params;    // params是回调函数中传入的信息
    cout << ", search result words is : ";

    for (const auto& word : words) {
        cout << word << " ";
    }
    cout << "" << endl;
}


/**
 * 异步多线程处理
 * @return
 */
int demo_asyncMultiThreadSearch() {
    CAISS_FUNCTION_BEGIN

    vector<void *> hdlsVec;
    for (int i = 0; i < max_thread_num_ ; i++) {
        void *handle = nullptr;
        ret = CAISS_CreateHandle(&handle);
        CAISS_FUNCTION_CHECK_STATUS
        hdlsVec.push_back(handle);    // 多个handle组成的vector
        ret = CAISS_Init(handle, CAISS_MODE_PROCESS, dist_type_, dim_, model_path_, dist_func_);
        CAISS_FUNCTION_CHECK_STATUS
    }

    int times = SEARCH_TIMES;
    while (times--) {
        for (auto& hdl : hdlsVec) {
            int i = (int)rand() % (int)WORDS.size();
            /* 在异步模式下，train，search等函数，不阻塞。但是会随着 */
            ret = CAISS_Search(hdl, (void *)(WORDS[i]).c_str(), search_type_, top_k_, searchCallbackFunc, WORDS[i].c_str());
            CAISS_FUNCTION_CHECK_STATUS
        }
    }

    int stop = 0;
    cin >> stop;    // 外部等待所有计算结束后，再结束流程

    for (auto t : hdlsVec) {
        ret = CAISS_destroyHandle(t);
        CAISS_FUNCTION_CHECK_STATUS
    }

    CAISS_FUNCTION_END
}


int syncSearch(void *handle) {
    CAISS_FUNCTION_BEGIN
    int times = SEARCH_TIMES;
    while (times--) {
        // 查询10000次，结束之后正常退出
        // 由于样本原因，可能会出现，输入的词语在模型中无法查到的问题。这种情况会返回非0的值
        int i = (int)rand() % (int)WORDS.size();
        ret = CAISS_Search(handle, (void *)(WORDS[i]).c_str(), search_type_, top_k_);
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
    }

    CAISS_FUNCTION_END
}


/**
 * 同步查询功能，上层开多个线程，每个线程使用不同的句柄来进行查询
 * @return
 */
int demo_syncMultiThreadSearch() {
    CAISS_FUNCTION_BEGIN

    vector<void *> hdlsVec;
    vector<std::future<int>> futVec;
    for (int i = 0; i < max_thread_num_ ; i++) {
        void *handle = nullptr;
        ret = CAISS_CreateHandle(&handle);
        CAISS_FUNCTION_CHECK_STATUS
        hdlsVec.push_back(handle);    // 多个handle组成的vector
        ret = CAISS_Init(handle, CAISS_MODE_PROCESS, dist_type_, dim_, model_path_, dist_func_);
        CAISS_FUNCTION_CHECK_STATUS

        std::future<int> fut = std::async(std::launch::async, syncSearch, handle);     // 在同步模式下，上层开辟线程去做多次查询的功能
        futVec.push_back(std::move(fut));
    }

    for (auto &fut : futVec) {
        ret = fut.get();
        CAISS_FUNCTION_CHECK_STATUS
    }

    for (auto &handle : hdlsVec) {
        ret = CAISS_destroyHandle(handle);
        CAISS_FUNCTION_CHECK_STATUS
    }

    CAISS_FUNCTION_END
}