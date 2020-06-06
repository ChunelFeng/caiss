//
// Created by Chunel on 2020/5/24.
//

#include <string>

#include <time.h>
#include "HnswProc.h"


#include <unordered_map>
#include <map>
#include "./boost/bimap/bimap.hpp"
typedef boost::bimaps::bimap<int, int> BOOST_INT_BIMAP;

using namespace std;

const static string TEST_MODEL_PATH = "test.ann";
const static string TEST_FILE_PATH = "test.txt";
const static unsigned int DIM = 4;
const static ANN_DISTANCE_TYPE DISTANCE_TYPE = ANN_DISTANCE_EUC;
const static unsigned int TOP_K = 2;

int train() {
    int ret = 0;
    auto proc = new HnswProc();
    ret = proc->init(ANN_MODE_TRAIN, DISTANCE_TYPE, DIM, TEST_MODEL_PATH.data(), 0);
    ret = proc->train(TEST_FILE_PATH.data(), 10, 0, 0, 1,1,1,1,1);

    delete proc;
    return ret;
}

int search() {
    int ret = 0;
    auto proc = new HnswProc();

    ret = proc->init(ANN_MODE_PROCESS, DISTANCE_TYPE, DIM, TEST_MODEL_PATH.data(), 0);
    ANN_FUNCTION_CHECK_STATUS
    vector<float> ist = {0,1.8,0,0};
    ret = proc->insert(ist.data(), "asd", ANN_INSERT_OVERWRITE);
    ANN_FUNCTION_CHECK_STATUS

    ret = proc->save(nullptr);
    ANN_FUNCTION_CHECK_STATUS

    vector<float> vec = {0,1.1,0,0};
    ret = proc->search(vec.data(), TOP_K, ANN_SEARCH_FAST);
    ANN_FUNCTION_CHECK_STATUS

    unsigned int size = 0;
    ret = proc->getResultSize(size);
    ANN_FUNCTION_CHECK_STATUS

    char* result = new char[size + 1];
    memset(result, 0, size+1);
    ret = proc->getResult(result, size);
    ANN_FUNCTION_CHECK_STATUS

    cout << result << endl;
    delete[] result;

    delete proc;
    return ret;
}

int insert() {


    return 0;
}

int bimapTest() {
//    tree map find 9999999(一千万) times need 6557 mil seconds.
//    hash map find 9999999 times need 1052 mil seconds.
//    bimap left find 9999999 times need 8083 mil seconds.
//    bimap right find 9999999 times need 8041 mil seconds.
    int total = 9999999;
    std::unordered_map<int, int> unmap;
    std::map<int, int> treemap;
    BOOST_INT_BIMAP bimap;
    for (int i = 0; i < total; ++i) {
        treemap.insert(make_pair(i, i));
        unmap.insert(make_pair(i, i));
        bimap.insert(BOOST_INT_BIMAP::value_type(i, i));
    }

    cout << "start clock " << endl;

    clock_t start , ends;

    start = clock();
    for (int j = 0; j < total; ++j) {
        treemap.find(j);
    }
    ends = clock();
    cout << "tree map find 9999999 times need " << ends - start << " mil seconds." << endl;

    start = clock();
    for (int j = 0; j < total; ++j) {
         unmap.find(j);
    }
    ends = clock();
    cout << "hash map find 9999999 times need " << ends - start << " mil seconds." << endl;

    start = clock();
    for (int k = 0; k < total; k++) {
        bimap.left.find(k);
    }
    ends = clock();
    cout << "bimap left find 9999999 times need " << ends - start << " mil seconds." << endl;

    start = clock();
    for (int k = 0; k < total; k++) {
        bimap.right.find(k);
    }
    ends = clock();
    cout << "bimap right find 9999999 times need " << ends - start << " mil seconds." << endl;

    return 0;
}

int main() {

    int ret = 0;
    //ret = train();
    ret = search();

    return 0;
}