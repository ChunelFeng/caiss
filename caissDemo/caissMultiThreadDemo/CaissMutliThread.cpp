//
// Created by Chunel on 2020/6/20.
//

#include <thread>
#include <chrono>
#include <fstream>
#include <functional>
#include <windows.h>
#include <dbghelp.h>
#include <windef.h>
#include "../CaissDemoInclude.h"

void __attribute__((stdcall)) ApplicationCrashHandler(LPCWSTR lpstrDumpFilePathName, EXCEPTION_POINTERS *pException)
{
    // 创建Dump文件
    //
    HANDLE hDumpFile = CreateFile(reinterpret_cast<LPCSTR>(lpstrDumpFilePathName), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    // Dump信息
    //
    MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
    dumpInfo.ExceptionPointers = pException;
    dumpInfo.ThreadId = GetCurrentThreadId();
    dumpInfo.ClientPointers = TRUE;

    CloseHandle(hDumpFile);
}


void STDCALL searchCallbackFunc(CAISS_LIST_STRING& words, CAISS_LIST_FLOAT& distances, const void *params) {


    char *p = (char *)params;
    if (words.front() != std::string(p)) {

        for (auto& a : words) {
            cout << a + "," ;
        }
        cout << "====" << p <<endl;
    } else {
        cout << "*******************" << endl;
    }

    return;
}

vector<string> words = {"hello", "world", "test", "coder", "yes", "no", "thank", "boy", "cow", "computer"};

int func(void *handle, int i) {

    cout << "handle : " << handle << " enter function" << endl;
    //std::function<void(CAISS_LIST_STRING& words, CAISS_LIST_FLOAT& distances, const void *params)> cbFunc = searchCallbackFunc;
    //std::ofstream wt("./log/" + std::to_string(i) + ".log", std::ios_base::out);

    while (true) {
        CAISS_FUNCTION_BEGIN


        int pp = ((int)rand() + i)%10;

        //cout << "enter search function ... word : " << words[pp] << endl;
        ret = CAISS_Search(handle, (void *)(words[pp]).c_str(), search_type_, top_k_, searchCallbackFunc, words[pp].c_str());

        this_thread::sleep_for(chrono::milliseconds(1));
        CAISS_FUNCTION_CHECK_STATUS
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

    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);

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