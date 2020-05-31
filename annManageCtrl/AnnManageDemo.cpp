//
// Created by Chunel on 2020/5/27.
//

#include <iostream>
#include <thread>
#include <vector>
#include <fstream>
#include <chrono>
#include "AnnManageProc.h"
#include "syncManage/SyncManageProc.h"

using namespace std;

const static string MODEL_PATH = "test.ann";


void func(int x, AnnManageProc* ptr) {

    ofstream outfile;

    string fileName = "./logs/" + std::to_string(x) +  ".txt";
    outfile.open(fileName.c_str());

    int ret = 0;
    int times = 100;

    void* handle = nullptr;
    ret = ptr->createHandle(&handle);

    ret = ptr->init(handle, ANN_MODE_PROCESS, ANN_DISTANCE_EUC, 4, MODEL_PATH.c_str(), 0);
    for (int i = 0; i < times; i++) {

        vector<ANN_FLOAT> query = {0, float(x), 0, 0};
        ret = ptr->search(handle, query.data(), 2);

        unsigned int size = 0;
        ret = ptr->getResultSize(handle, size);

        char *result = new char[size + 1];
        memset(result, 0, size + 1);
        ret = ptr->getResult(handle, result, size);
        outfile << result << endl;
        delete[] result;
    }

    ret = ptr->destroyHandle(handle);
    outfile.close();
}

int main() {

    cout << "== start ==" << endl;
    unsigned int size = 10;
    AnnManageProc* ptr = new SyncManageProc(size, ANN_ALGO_HNSW);

    auto beginTime = std::chrono::high_resolution_clock::now();
    std::vector<std::thread> workers;
    for (int i = 0; i < size; ++i) {
        workers.emplace_back(func, i, ptr);
    }

    for (auto &w : workers) {
        w.join();
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto elapsedTime= std::chrono::duration_cast<std::chrono::seconds>(endTime - beginTime);
    cout << elapsedTime.count() << " second. " << endl;

    cout << "== stop ==" << endl;
    return 0;
}
