//
// Created by Chunel on 2020/5/24.
//

#include <iostream>
#include <thread>
#include "RWLock.h"

using namespace std;

static RWLock* rw = new RWLock();
int cur = 0;
int size = 5000;

void readFunc() {
    for (int i = 0; i < size; i++) {
        rw->readLock();
        cout << "read : " << cur << endl;
        rw->readUnlock();
    }
}

void writeFunc() {

    for (int i = 0; i < size; i++) {
        rw->writeLock();
        cur ++;
        cout << "write : " << cur << endl;
        rw->writeUnlock();
    }

}

int main() {
    thread r_thd(readFunc);
    thread w_thd(writeFunc);

    r_thd.join();
    w_thd.join();

    return 0;
}
