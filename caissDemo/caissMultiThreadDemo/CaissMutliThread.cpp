//
// Created by Chunel on 2020/6/20.
//

#include <thread>
#include <chrono>
#include <list>
#include <fstream>
#include "../CaissDemoInclude.h"



int func(void *handle, int i) {

    cout << "handle : " << handle << " enter function" << endl;
    std::ofstream wt("./log/" + std::to_string(i) + ".log", std::ios_base::out);

    while (true) {
        CAISS_FUNCTION_BEGIN

        vector<string> words = {"hello", "world", "test", "coder", "yes", "no", "thank", "boy", "cow", "computer"};

        int pp = rand() % 10 + i;

        cout << "enter search function ... word : " << words[pp] << endl;
        ret = CAISS_Search(handle, (void *)(words[pp]).c_str(), search_type_, top_k_);

        this_thread::sleep_for(chrono::milliseconds(100));
        CAISS_FUNCTION_CHECK_STATUS

        unsigned int size = 0;
        while (1) {
            ret = CAISS_getResultSize(handle, size);
            if (CAISS_RET_OK == ret) {
                break;
            }
            this_thread::sleep_for(chrono::milliseconds(100));
        }

        char *result = new char[size + 1];
        memset(result, 0, size + 1);
        ret = CAISS_getResult(handle, result, size);
        CAISS_FUNCTION_CHECK_STATUS
        string tmp = words[pp] + "----" + result + "\n";
        //std::cout << handle << "----" << words[i] << "----" << result << std::endl;
        wt << tmp << endl;
        delete [] result;
    }

    cout << "handle " << handle << " leave funciont" << endl;
    return 0;
}


int demo_multiThreadTrain() {
    CAISS_FUNCTION_BEGIN
    // 注：训练不支持多线程功能
    CAISS_FUNCTION_END
}

int demo_asyncMultiThreadSearch() {
    CAISS_FUNCTION_BEGIN

    void *handle = nullptr;
    vector<void *> hdls;
    for (int i = 0; i < max_thread_num_ ; i++) {
        ret = CAISS_CreateHandle(&handle);
        CAISS_FUNCTION_CHECK_STATUS
        hdls.push_back(handle);
        ret = CAISS_Init(handle, CAISS_MODE_PROCESS, dist_type_, dim_, model_path_, dist_func_);
        CAISS_FUNCTION_CHECK_STATUS
    }


    vector<thread> thds;
    for (int i = 0; i < hdls.size(); i++) {
        thds.push_back(thread(func, hdls[i], i));
    }

    for (int i = 0; i < hdls.size(); i++) {
        thds[i].join();
    }

    for (auto t : hdls) {
        ret = CAISS_destroyHandle(t);
        CAISS_FUNCTION_CHECK_STATUS
    }

    CAISS_FUNCTION_END
}