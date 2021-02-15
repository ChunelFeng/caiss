//
// Created by Chunel on 2020/6/20.
// 多线程版本demo，介绍同步/异步多线程调用方法
//


#include <functional>
#include <future>

#include "../CaissDemoInclude.h"

const static vector<string> WORDS = {"this", "is", "an", "open", "source", "project", "and", "hope", "it", "will", "be", "useful", "for", "you", "best", "wishes"};
static int SEARCH_TIMES = 1000000;

void STDCALL searchCallbackFunc(const char *query,
                                const CAISS_STRING_ARRAY& words,
                                const CAISS_FLOAT_ARRAY& distances,
                                const void *params) {
    cout << "The query word is [" << query << "], and the match words may be : ";
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
    CAISS_ECHO("enter demo_asyncMultiThreadSearch function ...");

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
        for (int i = 0; i < hdlsVec.size(); i++) {
            int num = (int)(rand() + i) % (int)WORDS.size();
            /* 在异步模式下，train，search等函数，不阻塞。但进程结束时，会自动结束所有未完成的任务 */
            CAISS_Search(hdlsVec[i], (void *)(WORDS[num]).c_str(), search_type_, top_k_, filter_edit_distance_, searchCallbackFunc, WORDS[num].c_str());
        }
    }

    int stop = 0;
    cin >> stop;    // 外部等待所有计算结束后，再结束流程

    for (auto &t : hdlsVec) {
        ret = CAISS_DestroyHandle(t);
        CAISS_FUNCTION_CHECK_STATUS
    }

    CAISS_FUNCTION_END
}


int syncSearch(void *handle) {
    CAISS_FUNCTION_BEGIN
    int times = SEARCH_TIMES;
    while (times--) {
        // 查询SEARCH_TIMES次，结束之后正常退出
        int i = (int)rand() % (int)WORDS.size();
        ret = CAISS_Search(handle, (void *)(WORDS[i]).c_str(), search_type_, top_k_, filter_edit_distance_);
    }

    CAISS_FUNCTION_END
}


/**
 * 同步查询功能，上层开多个线程，每个线程使用不同的句柄来进行查询
 * @return
 */
int demo_syncMultiThreadSearch() {
    CAISS_FUNCTION_BEGIN
    CAISS_ECHO("enter demo_syncMultiThreadSearch function ...");

    vector<void *> hdlsVec;
    vector<std::future<int>> futVec;
    for (int i = 0; i < max_thread_num_ ; i++) {
        void *handle = nullptr;
        ret = CAISS_CreateHandle(&handle);
        CAISS_FUNCTION_CHECK_STATUS
        hdlsVec.push_back(handle);    // 多个handle组成的vector
        ret = CAISS_Init(handle, CAISS_MODE_PROCESS, dist_type_, dim_, model_path_, dist_func_);
        CAISS_FUNCTION_CHECK_STATUS
    }

    auto start = clock();
    for (auto &handle : hdlsVec) {
        std::future<int> fut = std::async(std::launch::async, syncSearch, handle);     // 在同步模式下，上层开辟线程去做多次查询的功能
        futVec.push_back(std::move(fut));
    }

    for (auto &fut : futVec) {
        ret = fut.get();
        CAISS_FUNCTION_CHECK_STATUS
    }

    for (auto &handle : hdlsVec) {
        ret = CAISS_DestroyHandle(handle);
        CAISS_FUNCTION_CHECK_STATUS
    }

    CAISS_FUNCTION_END
}