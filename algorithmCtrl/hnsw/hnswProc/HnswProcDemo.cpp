//
// Created by Chunel on 2020/5/24.
//

#include <string>
#include "HnswProc.h"

using namespace std;

const static string TEST_MODEL_PATH = "test.ann";
const static string TEST_FILE_PATH = "test.txt";
const static unsigned int DIM = 4;
const static ANN_DISTANCE_TYPE DISTANCE_TYPE = ANN_DISTANCE_EUC;
const static unsigned int TOP_K = 3;

int main() {

    int ret = 0;
    auto proc = new HnswProc();
    ret = proc->init(ANN_MODE_TRAIN, DISTANCE_TYPE, DIM, TEST_MODEL_PATH.data(), 0);

    ret = proc->train(TEST_FILE_PATH.data(), 10, 0, 0, 1,1,1,1,1);

    ret = proc->init(ANN_MODE_PROCESS, DISTANCE_TYPE, DIM, TEST_MODEL_PATH.data(), 0);
    vector<float> vec = {0,1.2,0,0};
    ret = proc->search(vec.data(), TOP_K, ANN_SEARCH_FAST);

    unsigned int size = 0;
    ret = proc->getResultSize(size);

    char* result = new char[size + 1];
    memset(result, 0, size+1);
    ret = proc->getResult(result, size);
    cout << result << endl;
    delete[] result;

    delete proc;
    return 0;
}