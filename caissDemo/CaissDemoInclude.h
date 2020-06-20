//
// Created by Chunel on 2020/6/20.
//

#ifndef CAISS_CAISSDEMOINCLUDE_H
#define CAISS_CAISSDEMOINCLUDE_H

#include <string>
#include "../utilsCtrl/UtilsInclude.h"
#include "../caissLib/CaissLib.h"
#include "caissSimpleDemo/CaissSimple.h"
#include "caissMultiThreadDemo/CaissMultiThread.h"


#define max_thread_num_ (4)
#define algo_type_ (CAISS_ALGO_HNSW)
#define manage_type_ (CAISS_MANAGE_ASYNC)
#define mode_ (CAISS_MODE_PROCESS)
#define dist_type_ (CAISS_DISTANCE_INNER)
#define dim_ (768)
#define model_path_ ("../../doc/libDemo.caiss")
#define dist_func_ (nullptr)
#define info_ ("decent")
#define search_type_ (CAISS_SEARCH_WORD)
#define top_k_ (5)

#define data_path_ ("../../doc/bert_71290words_768dim.txt")
#define max_data_size_ (100000)
#define normalize_ (CAISS_TRUE)
#define max_index_size_ (64)
#define precision_ (0.95)
#define fast_rank_ (5)
#define real_rank_ (5)
#define step_ (1)
#define max_epoch_ (3)
#define show_span_ (1000)



#endif //CAISS_CAISSDEMOINCLUDE_H
