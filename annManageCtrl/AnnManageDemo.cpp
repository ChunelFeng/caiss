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



void func(int x, AnnManageProc* ptr) {

    ofstream outfile;

    //string fileName = "./logs/" + std::to_string(x) +  ".txt";
    //outfile.open(fileName.c_str());

    int ret = 0;
    int times = 10;

    void* handle = nullptr;
    string path = "test.ann";
    ret = ptr->createHandle(&handle);

    //outfile << x  << " thread, " << i << " times, create : " << ret << ", " << handle << endl;

    ret = ptr->init(handle, ANN_MODE_PROCESS, ANN_DISTANCE_EUC, 4, path.c_str(), 0);
    for (int i = 0; i < times; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));


        vector<ANN_FLOAT> query = {0, float(x), 0, 0};
        ret = ptr->search(handle, query.data(), 2);

        unsigned int size = 0;
        ret = ptr->getResultSize(handle, size);

        char *result = new char[size + 1];
        memset(result, 0, size + 1);
        ret = ptr->getResult(handle, result, size);
        //outfile << x << " thread, result is : " << result << endl;
        cout << result <<endl;
        delete[] result;

        //outfile << x << " thread, " << i << " times, destroy : " << ptr->destroyHandle(handle) << endl;
    }

    ret = ptr->destroyHandle(handle);
    outfile.close();
}

int main() {

    cout << "== start ==" << endl;
    unsigned int size = 5;
    AnnManageProc* ptr = new SyncManageProc(size);

    std::vector<std::thread> workers;
    for (int i = 0; i < size; ++i) {
        workers.emplace_back(func, i, ptr);
    }

    for (auto &w : workers) {
        w.join();
    }

    cout << "== stop ==" << endl;
    return 0;
}
