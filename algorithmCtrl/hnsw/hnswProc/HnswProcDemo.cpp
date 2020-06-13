//
// Created by Chunel on 2020/5/24.
//

#include <string>

#include <windows.h>
#include <algorithm>
#include <time.h>
#include "HnswProc.h"


#include <unordered_map>
#include <map>
#include "./boost/bimap/bimap.hpp"
typedef boost::bimaps::bimap<int, int> BOOST_INT_BIMAP;

using namespace std;

const static string TEST_MODEL_PATH = "train-ex";
const static string TEST_FILE_PATH = "bert_71290words_768dim.txt";
//const static string TEST_FILE_PATH = "result-71290words-128dim.txt";
//const static string TEST_FILE_PATH = "result-1000words-128dim.txt";
const static int ACTION = 0;    // 0-train. 1-search. 2-demo
const static unsigned int DIM = 768;
const static CAISS_DISTANCE_TYPE DISTANCE_TYPE = CAISS_DISTANCE_INNER;
const static CAISS_BOOL NORMALIZE = 1;
const static unsigned int TOP_K = 5;
const static std::string SEARCH_WORD = "think";
const static CAISS_SEARCH_TYPE SEARCH_TYPE = CAISS_SEARCH_WORD;
//const static vector<float> VEC = {-1.2995677, -0.15858799, 0.49073297, 0.86445516, -0.6184684, 0.77375257, -0.6075031, -0.23887867, -0.4722771, -0.07010775, 0.18131949, -0.3215046, -0.56551075, -0.025499033, -0.14106233, 0.19489725, 0.23328665, -0.11499162, 0.69570893, -0.39448798, 0.772822, -0.36182213, 0.6397236, 1.3442292, -0.70384866, 0.79472506, 0.023136038, -0.5285814, -0.40006396, 1.1421493, 0.9105783, 0.33832657, -0.03106412, 0.46195656, 0.18878146, 0.22560893, 0.16511455, -0.7164479, -0.5678104, 0.5892788, 0.44944057, -0.103624694, -0.16079843, 0.06063378, 0.4381161, 0.12510821, 0.20019467, 0.26359266, -0.2298484, -0.68666196, 0.7693924, -0.59070027, 0.6527828, -0.6371826, 0.39478585, 0.13500752, 0.44513932, 0.82218814, -0.15528621, 0.009237586, 0.06952912, 0.11235138, -0.210345, -0.68272674, 0.790048, -0.28668272, 0.4260615, -0.20765416, -0.0603083, -0.14223874, -0.10975164, -0.031348865, 0.041884407, 1.5536473, -0.016608158, 0.08571435, -0.08582062, 0.4492224, 0.23094673, -0.9216085, 0.64852345, 0.48624858, 0.64585173, 0.32146472, -0.17158557, 0.33764848, 0.16976501, 0.5952861, -0.4807511, 0.19375898, -0.78834367, -0.34773567, -0.41534054, -0.78047657, 0.56956273, 0.14834143, 0.25622267, 0.015336277, -0.22903873, 0.796972, 0.8760641, -0.026135948, -0.017635783, -0.28747213, 0.5585467, 0.52534544, -0.35134995, -0.05430208, 0.2945827, -0.9133815, 0.66416454, -0.6894948, -0.78729653, 0.48896956, 0.22490738, 0.14767036, -0.46799278, -0.1390024, -0.59413934, -0.36416388, -1.4118644, 0.28838876, 0.84712005, -0.44438282, 0.56619006, -0.1188119, -0.20578334, 0.5524768};
const static vector<float> VEC = {1.1, 0,0,0};

static int train() {
    int ret = 0;
    auto proc = new HnswProc();
    ret = proc->init(CAISS_MODE_TRAIN, DISTANCE_TYPE, DIM, TEST_MODEL_PATH.data(), 0);
    CAISS_FUNCTION_CHECK_STATUS

    ret = proc->train(TEST_FILE_PATH.data(), 100000, NORMALIZE, 64, 0.98, 10, 10, 1, 5, 100);
    cout << "train return : " << ret <<endl;

    CAISS_FUNCTION_CHECK_STATUS

    delete proc;
    return ret;
}

static int search(string word) {
    int ret = 0;
    auto proc = new HnswProc();

    ret = proc->init(CAISS_MODE_PROCESS, DISTANCE_TYPE, DIM, TEST_MODEL_PATH.data(), 0);
    CAISS_FUNCTION_CHECK_STATUS

    //ret = proc->search((void *)VEC.data(), CAISS_SEARCH_QUERY, TOP_K);

    ret = proc->search((void *)word.c_str(), SEARCH_TYPE, TOP_K);
    CAISS_FUNCTION_CHECK_STATUS

    unsigned int size = 0;
    ret = proc->getResultSize(size);
    CAISS_FUNCTION_CHECK_STATUS

    char* result = new char[size + 1];
    memset(result, 0, size+1);
    ret = proc->getResult(result, size);
    CAISS_FUNCTION_CHECK_STATUS

    //cout << result << endl;
    delete[] result;

    int count = 0;
    cout << "The recommend words are : ";
    for (auto x : proc->result_words_) {
        if (count < 5 && count > 0) {
            cout << x << ", ";
        } else if (count == 5) {
            cout << x << "." << endl;
        }
        count++;
    }


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


int mainFunc() {

    SetConsoleTitle("ANN");
    int ret = 0;
    if (ACTION == 0) {
        ret = train();
    } else if (ACTION == 1) {
        ret = search(SEARCH_WORD);
    } else if (ACTION == 2) {
        cout << "**** Please enter a word, we will recommend 5 similar words for you. enter : ";
        int times = 100000;
        while (times--)
        {
            string ipt;
            cin >> ipt;
            transform(ipt.begin(), ipt.end(), ipt.begin(), ::tolower);

            bool isWord = true;
            for (char i : ipt) {
                if (i > 'z' || i < 'a') {
                    isWord = false;
                    break;
                }
            }

            if (!isWord) {
                cout << "**** [" << ipt << "] is not a English word." << endl;
                cout << "**** Please enter a word, we will recommend 5 similar words for you. enter : ";
                continue;
            }

            ret = search(ipt);



            if (CAISS_RET_NO_WORD == ret) {
                cout << "" << endl;
                cout << "**** Fuck,  [" << ipt << "] is not a word, please try again... " << endl;
                cout << "**** Please enter a word, we will recommend 5 similar words for you. enter : ";
            } else if (CAISS_RET_OK == ret) {
                cout << "" << endl;
                cout << "**** Funny ? Please enter a word again, we will recommend 5 similar words for you. enter : ";
            } else {
                cout << "" << endl;
                cout << "**** Sorry, these is something wrong." << endl;
                cout << "**** Please try again. enter : " << endl;
            }
        }
    }

    return 0;
}



int main() {
    mainFunc();

    return 0;
}