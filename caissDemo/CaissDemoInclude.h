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


#define max_thread_num_ (1)
#define manage_type_ (CAISS_MANAGE_SYNC)

#define algo_type_ (CAISS_ALGO_HNSW)
#define mode_ (CAISS_MODE_PROCESS)
#define dist_type_ (CAISS_DISTANCE_INNER)
#define dim_ (768)
#define model_path_ ("../../doc/demo_2500words_768dim.caiss")
#define dist_func_ (nullptr)
#define info_ ("source")
#define search_type_ (CAISS_SEARCH_WORD)
#define top_k_ (5)

#define data_path_ ("../../doc/demo_2500words_768dim.txt")
#define max_data_size_ (5000)    // 不小于文本最大行数。建议设定为文本行数的1.5倍~2倍，方便今后插入信息。
#define normalize_ (CAISS_TRUE)
#define max_index_size_ (64)
#define precision_ (0.95)
#define fast_rank_ (5)
#define real_rank_ (5)
#define step_ (1)
#define max_epoch_ (1)
#define show_span_ (1000)


#endif //CAISS_CAISSDEMOINCLUDE_H
