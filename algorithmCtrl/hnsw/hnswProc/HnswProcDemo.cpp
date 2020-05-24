//
// Created by Chunel on 2020/5/24.
//

#include <string>
#include "HnswProc.h"

using namespace std;

const static string TEST_MODEL_PATH = "test.ann";
const static string TEST_FILE_PATH = "test.txt";


int main() {


    HnswProc* proc = new HnswProc();
    proc->init(ANN_MODE_TRAIN, 4, TEST_MODEL_PATH.data(), 0);

    proc->train(TEST_FILE_PATH.data(), 10, 1, 0, 1,1,1,1,1);

    vector<float> vec = {0,1,0,0};
    proc->search(vec.data(), 1, ANN_SEARCH_FAST);

    delete proc;
    return 0;
}