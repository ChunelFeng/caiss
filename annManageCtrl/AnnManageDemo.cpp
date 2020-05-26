//
// Created by Chunel on 2020/5/27.
//

#include <iostream>
#include <thread>
#include "AnnManageProc.h"
#include "syncManage/SyncManageProc.h"

using namespace std;

AnnManageProc* ptr = new SyncManageProc(1000);
int times = 10000;

void func(int x) {
    for (int i = 0; i < times; i++) {
        void* handle = nullptr;
        cout << i << " create : " << ptr->createHandle(&handle) << ", " << handle << endl;
        cout << i << " destroy : " << ptr->destroyHandle(handle) << endl;
    }
}

int main() {

    thread th1(func, 1);
    thread th2(func, 2);
    thread th3(func, 3);
    thread th4(func, 4);
    thread th5(func, 5);
    thread th6(func, 6);
    thread th7(func, 7);
    thread th8(func, 8);

    th1.join();
    th2.join();
    th3.join();
    th4.join();
    th5.join();
    th6.join();
    th7.join();
    th8.join();

    cout << "finish" << endl;
    return 0;
}
