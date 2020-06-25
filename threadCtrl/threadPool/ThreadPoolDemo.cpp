//
// Created by Chunel on 2020/6/20.
//

#include <iostream>
#include <chrono>
#include "ThreadPool.h"

using namespace std;

int func() {
    cout << " working void ansycSearch ..." << endl;
    return 0;
}

int func2(int i) {
    cout << " working int ansycSearch..." << endl;
    return 0;
}

int func3(int i, int j, int k) {
    cout << "ansycSearch 3" << endl;
    return 0;
}


int main() {
    ThreadPool pool(3);
    pool.start();

    for (int i = 0; i < 100; i++) {
        pool.appendTask(std::bind(func3, 2,3,1));
        //pool.appendTask(test);
    }

    this_thread::sleep_for(chrono::milliseconds(1000));
    pool.stop();

    return 0;
}