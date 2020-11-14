#pragma once

#include "visited_list_pool.h"
#include "hnswlib.h"
#include <random>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <atomic>
#include <list>
#include <unordered_set>
#include <unordered_map>
#include "./boost/bimap/bimap.hpp"

#include "../../../utilsCtrl/UtilsInclude.h"

namespace hnswlib {
    typedef unsigned int tableint;
    typedef unsigned int linklistsizeint;
    typedef boost::bimaps::bimap<labeltype, std::string> BOOST_BIMAP;

    template<typename dist_t>
    class HierarchicalNSW : public AlgorithmInterface<dist_t> {
    public:
        HierarchicalNSW(SpaceInterface<dist_t> *s) {
        }

        HierarchicalNSW(SpaceInterface<dist_t> *s, const std::string &location, TrieProc* trie, size_t max_elements=0) {
            loadIndex(location, s, trie, max_elements);
        }

        HierarchicalNSW(SpaceInterface<dist_t> *s, size_t max_elements, int normalize = 0,
                unsigned int index_size = 64, size_t M = 32, size_t ef = 100,
                size_t ef_construction = 100, size_t random_seed = 100) :
                link_list_locks_(max_elements), element_levels_(max_elements) {

            max_elements_ = max_elements;

            data_size_ = s->get_data_size();
            fstdistfunc_ = s->get_dist_func();
            dist_func_param_ = s->get_dist_func_param();
            M_ = M;
            maxM_ = M_;
            maxM0_ = M_ * 2;
            ef_construction_ = std::max(ef_construction, M_);
            ef_ = ef;

            level_generator_.seed(random_seed);

            size_links_level0_ = maxM0_ * sizeof(tableint) + sizeof(linklistsizeint);
            size_data_per_element_ = size_links_level0_ + data_size_ + sizeof(labeltype);
            offsetData_ = size_links_level0_;
            label_offset_ = size_links_level0_ + data_size_;
            offsetLevel0_ = 0;

            data_level0_memory_ = (char *) malloc(max_elements_ * size_data_per_element_);
            if (data_level0_memory_ == nullptr)
                throw std::runtime_error("Not enough memory");

            cur_element_count_ = 0;

            visited_list_pool_ = new VisitedListPool(1, max_elements);    // 这里是一个线程池，但是只开了一个线程。我在外面做线程了
            //initializations for special treatment of the first node
            enterpoint_node_ = -1;
            maxlevel_ = -1;

            linkLists_ = (char **) malloc(sizeof(void *) * max_elements_);
            size_links_per_element_ = maxM_ * sizeof(tableint) + sizeof(linklistsizeint);
            mult_ = 1 / log(1.0 * M_);
            revSize_ = 1.0 / mult_;

            normalize_ = normalize;
            per_index_size_ = index_size;
            index_ptr_ = (char *)malloc(max_elements_ * per_index_size_);    // 分配空间，保存具体index信息，训练的时候加载的方法
            memset(index_ptr_, 0, max_elements_ * per_index_size_);

            ignore_info_ = (char *)malloc(max_elements_ * per_index_size_);
            memset(ignore_info_, 0, max_elements_ * per_index_size_);    // 清空信息
        }

        struct CompareByFirst {
            constexpr bool operator()(std::pair<dist_t, tableint> const &a,
                                      std::pair<dist_t, tableint> const &b) const noexcept {
                return a.first < b.first;
            }
        };

        ~HierarchicalNSW() {
            free(data_level0_memory_);
            for (tableint i = 0; i < cur_element_count_; i++) {
                if (element_levels_[i] > 0)
                    free(linkLists_[i]);
            }

            if (index_ptr_) {
                free(index_ptr_);    // 是free，因为是malloc出来的内容
                index_ptr_ = nullptr;
            }

            if (ignore_info_) {
                free(ignore_info_);
                ignore_info_ = nullptr;
            }

            free(linkLists_);
            delete visited_list_pool_;
        }

        size_t max_elements_;
        size_t cur_element_count_;
        size_t size_data_per_element_;
        size_t size_links_per_element_;

        size_t M_;
        size_t maxM_;
        size_t maxM0_;
        size_t ef_construction_;

        int normalize_;    // 是否是标准化的内容
        int ignore_word_size_;    // 忽略的词语的数量
        int placeholder_0_;    // 添加placeholder信息
        int placeholder_1_;
        int placeholder_2_;
        int placeholder_3_;
        int placeholder_4_;
        int placeholder_5_;
        int placeholder_6_;
        int placeholder_7_;

        double mult_, revSize_;
        int maxlevel_;

        VisitedListPool *visited_list_pool_;
        std::mutex cur_element_count_guard_;

        std::vector<std::mutex> link_list_locks_;
        tableint enterpoint_node_;

        size_t size_links_level0_;
        size_t offsetData_, offsetLevel0_;

        char *data_level0_memory_;
        char **linkLists_;    // linkLists_[i][*]中，是第i个node对应的的跳表中，对应的邻居点
        std::vector<int> element_levels_;    // 是一个size=最大节点数的vector，每个值表示第i个节点，对应多少层

        size_t data_size_;
        size_t label_offset_;
        DISTFUNC<dist_t> fstdistfunc_;
        void *dist_func_param_;
        std::unordered_map<labeltype, tableint> label_lookup_;
        std::default_random_engine level_generator_;

        char *index_ptr_;    // 用于存放所有单词的地方
        unsigned int per_index_size_;
        BOOST_BIMAP index_lookup_;

        char *ignore_info_;    // 用于存放被忽略的信息（当调用save的时候，被加入模型）

        /**
         * 获取当前
         * @param internal_id
         * @return
         */
        inline labeltype getExternalLabel(tableint internal_id) const {
            labeltype return_label;
            // 这里的sizeof(labeltype)=8，相当于label_offset + labeltype = size_data_per_element_
            memcpy(&return_label,(data_level0_memory_ + internal_id * size_data_per_element_ + label_offset_), sizeof(labeltype));
            return return_label;
        }

        inline labeltype *getExternalLabeLp(tableint internal_id) const {
            return (labeltype *) (data_level0_memory_ + internal_id * size_data_per_element_ + label_offset_);
        }

        inline char *getDataByInternalId(tableint internal_id) const {
            return (data_level0_memory_ + internal_id * size_data_per_element_ + offsetData_);
        }

        int getRandomLevel(double reverse_size) {
            std::uniform_real_distribution<double> distribution(0.0, 1.0);
            double r = -log(distribution(level_generator_)) * reverse_size;
            return (int) r;
        }

        std::priority_queue<std::pair<dist_t, tableint>, std::vector<std::pair<dist_t, tableint>>, CompareByFirst>
        searchBaseLayer(tableint enterpoint_id, void *data_point, int layer) {
            VisitedList *vl = visited_list_pool_->getFreeVisitedList();
            vl_type *visited_array = vl->mass;
            vl_type visited_array_tag = vl->curV;

            std::priority_queue<std::pair<dist_t, tableint>, std::vector<std::pair<dist_t, tableint>>, CompareByFirst> top_candidates;
            std::priority_queue<std::pair<dist_t, tableint>, std::vector<std::pair<dist_t, tableint>>, CompareByFirst> candidateSet;
            dist_t dist = fstdistfunc_(data_point, getDataByInternalId(enterpoint_id), dist_func_param_);

            top_candidates.emplace(dist, enterpoint_id);
            candidateSet.emplace(-dist, enterpoint_id);
            visited_array[enterpoint_id] = visited_array_tag;
            dist_t lowerBound = dist;

            while (!candidateSet.empty()) {
                std::pair<dist_t, tableint> curr_el_pair = candidateSet.top();
                if ((-curr_el_pair.first) > lowerBound) {
                    break;
                }
                candidateSet.pop();

                tableint curNodeNum = curr_el_pair.second;

                std::unique_lock <std::mutex> lock(link_list_locks_[curNodeNum]);

                int *data;// = (int *)(linkList0_ + curNodeNum * size_links_per_element0_);
                if (layer == 0)
                    data = (int *) (data_level0_memory_ + curNodeNum * size_data_per_element_ + offsetLevel0_);
                else
                    data = (int *) (linkLists_[curNodeNum] + (layer - 1) * size_links_per_element_);
                int size = *data;
                tableint *datal = (tableint *) (data + 1);
        #ifdef USE_SSE
                _mm_prefetch((char *) (visited_array + *(data + 1)), _MM_HINT_T0);
                _mm_prefetch((char *) (visited_array + *(data + 1) + 64), _MM_HINT_T0);
                _mm_prefetch(getDataByInternalId(*datal), _MM_HINT_T0);
                _mm_prefetch(getDataByInternalId(*(datal + 1)), _MM_HINT_T0);
        #endif

                for (int j = 0; j < size; j++) {
                    tableint candidate_id = *(datal + j);
        #ifdef USE_SSE
                    _mm_prefetch((char *) (visited_array + *(datal + j + 1)), _MM_HINT_T0);
                    _mm_prefetch(getDataByInternalId(*(datal + j + 1)), _MM_HINT_T0);
        #endif
                    if (visited_array[candidate_id] == visited_array_tag) continue;
                    visited_array[candidate_id] = visited_array_tag;
                    char *currObj1 = (getDataByInternalId(candidate_id));

                    dist_t dist1 = fstdistfunc_(data_point, currObj1, dist_func_param_);
                    if (top_candidates.top().first > dist1 || top_candidates.size() < ef_construction_) {
                        candidateSet.emplace(-dist1, candidate_id);
        #ifdef USE_SSE
                        _mm_prefetch(getDataByInternalId(candidateSet.top().second), _MM_HINT_T0);
        #endif
                        top_candidates.emplace(dist1, candidate_id);
                        if (top_candidates.size() > ef_construction_) {
                            top_candidates.pop();
                        }
                        lowerBound = top_candidates.top().first;
                    }
                }
            }
            visited_list_pool_->releaseVisitedList(vl);

            return top_candidates;
        }

        /**
         * 在最下面一层查询，返回k个最近的元素
         * @param ep_id
         * @param data_point
         * @param ef
         * @return
         */
        std::priority_queue<std::pair<dist_t, tableint>, std::vector<std::pair<dist_t, tableint>>, CompareByFirst>
        searchBaseLayerST(tableint ep_id, const void *data_point, size_t ef) const {
            // 其中ep-id表示，当前是第几个节点；data-point是查询点的矩阵信息
            VisitedList *vl = visited_list_pool_->getFreeVisitedList();
            vl_type *visited_array = vl->mass;
            vl_type visited_array_tag = vl->curV;

            std::priority_queue<std::pair<dist_t, tableint>, std::vector<std::pair<dist_t, tableint>>, CompareByFirst> top_candidates;
            std::priority_queue<std::pair<dist_t, tableint>, std::vector<std::pair<dist_t, tableint>>, CompareByFirst> candidate_set;
            dist_t dist = fstdistfunc_(data_point, getDataByInternalId(ep_id), dist_func_param_);

            top_candidates.emplace(dist, ep_id);    // 放入当前的节点和query点的距离
            candidate_set.emplace(-dist, ep_id);
            visited_array[ep_id] = visited_array_tag;
            dist_t lower_bound = dist;

            while (!candidate_set.empty()) {

                std::pair<dist_t, tableint> current_node_pair = candidate_set.top();

                if ((-current_node_pair.first) > lower_bound) {     // current_node_pair标记了距离和点的index信息
                    break;
                }
                candidate_set.pop();

                tableint current_node_id = current_node_pair.second;
                int *data = (int *) (data_level0_memory_ + current_node_id * size_data_per_element_ + offsetLevel0_);
                int size = *data;    // size的值是dim，从第0层中，拿到数据信息。偏移后，拿到的应该是表示这个node-id有多少个邻居点的
        #ifdef USE_SSE
                _mm_prefetch((char *) (visited_array + *(data + 1)), _MM_HINT_T0);
                _mm_prefetch((char *) (visited_array + *(data + 1) + 64), _MM_HINT_T0);
                _mm_prefetch(data_level0_memory_ + (*(data + 1)) * size_data_per_element_ + offsetData_, _MM_HINT_T0);
                _mm_prefetch((char *) (data + 2), _MM_HINT_T0);
        #endif

                for (int j = 1; j <= size; j++) {
                    int candidate_id = *(data + j);
        #ifdef USE_SSE
                    _mm_prefetch((char *) (visited_array + *(data + j + 1)), _MM_HINT_T0);
                    _mm_prefetch(data_level0_memory_ + (*(data + j + 1)) * size_data_per_element_ + offsetData_,
                                 _MM_HINT_T0);
        #endif
                    if (!(visited_array[candidate_id] == visited_array_tag)) {

                        visited_array[candidate_id] = visited_array_tag;

                        char *currObj1 = (getDataByInternalId(candidate_id));
                        dist_t dist = fstdistfunc_(data_point, currObj1, dist_func_param_);

                        if (top_candidates.top().first > dist || top_candidates.size() < ef) {
                            candidate_set.emplace(-dist, candidate_id);
        #ifdef USE_SSE
                            _mm_prefetch(data_level0_memory_ + candidate_set.top().second * size_data_per_element_ +
                                         offsetLevel0_, _MM_HINT_T0);
        #endif

                            top_candidates.emplace(dist, candidate_id);

                            if (top_candidates.size() > ef) {
                                top_candidates.pop();
                            }
                            lower_bound = top_candidates.top().first;
                        }
                    }
                }
            }

            visited_list_pool_->releaseVisitedList(vl);
            return top_candidates;
        }

        void getNeighborsByHeuristic2(
                std::priority_queue<std::pair<dist_t, tableint>, std::vector<std::pair<dist_t, tableint>>, CompareByFirst> &top_candidates,
                const size_t M) {
            if (top_candidates.size() < M) {
                return;
            }
            std::priority_queue<std::pair<dist_t, tableint>> queue_closest;
            std::vector<std::pair<dist_t, tableint>> return_list;
            while (top_candidates.size() > 0) {
                queue_closest.emplace(-top_candidates.top().first, top_candidates.top().second);
                top_candidates.pop();
            }

            while (queue_closest.size()) {
                if (return_list.size() >= M)
                    break;
                std::pair<dist_t, tableint> curent_pair = queue_closest.top();
                dist_t dist_to_query = -curent_pair.first;
                queue_closest.pop();
                bool good = true;
                for (std::pair<dist_t, tableint> second_pair : return_list) {
                    dist_t curdist =
                            fstdistfunc_(getDataByInternalId(second_pair.second),
                                         getDataByInternalId(curent_pair.second),
                                         dist_func_param_);;
                    if (curdist < dist_to_query) {
                        good = false;
                        break;
                    }
                }
                if (good) {
                    return_list.push_back(curent_pair);
                }
            }

            for (std::pair<dist_t, tableint> curent_pair : return_list) {

                top_candidates.emplace(-curent_pair.first, curent_pair.second);
            }
        }


        linklistsizeint *get_linklist0(tableint internal_id) {
            return (linklistsizeint *) (data_level0_memory_ + internal_id * size_data_per_element_ + offsetLevel0_);
        };

        linklistsizeint *get_linklist0(tableint internal_id, char *data_level0_memory_) {
            return (linklistsizeint *) (data_level0_memory_ + internal_id * size_data_per_element_ + offsetLevel0_);
        };

        linklistsizeint *get_linklist(tableint internal_id, int level) {
            return (linklistsizeint *) (linkLists_[internal_id] + (level - 1) * size_links_per_element_);
        };

        void mutuallyConnectNewElement(void *data_point, tableint cur_c,
                                       std::priority_queue<std::pair<dist_t, tableint>, std::vector<std::pair<dist_t, tableint>>, CompareByFirst> top_candidates,
                                       int level) {

            size_t Mcurmax = level ? maxM_ : maxM0_;
            getNeighborsByHeuristic2(top_candidates, M_);
            if (top_candidates.size() > M_)
                throw std::runtime_error("Should be not be more than M_ candidates returned by the heuristic");

            std::vector<tableint> selectedNeighbors;
            selectedNeighbors.reserve(M_);
            while (top_candidates.size() > 0) {
                selectedNeighbors.push_back(top_candidates.top().second);
                top_candidates.pop();
            }
            {
                linklistsizeint *ll_cur;
                if (level == 0)
                    ll_cur = get_linklist0(cur_c);
                else
                    ll_cur = get_linklist(cur_c, level);

                if (*ll_cur) {
                    throw std::runtime_error("The newly inserted element should have blank link list");
                }
                *ll_cur = selectedNeighbors.size();
                tableint *data = (tableint *) (ll_cur + 1);


                for (size_t idx = 0; idx < selectedNeighbors.size(); idx++) {
                    if (data[idx])
                        throw std::runtime_error("Possible memory corruption");
                    if (level > element_levels_[selectedNeighbors[idx]])
                        throw std::runtime_error("Trying to make a link on a non-existent level");

                    data[idx] = selectedNeighbors[idx];

                }
            }
            for (size_t idx = 0; idx < selectedNeighbors.size(); idx++) {

                std::unique_lock <std::mutex> lock(link_list_locks_[selectedNeighbors[idx]]);


                linklistsizeint *ll_other;
                if (level == 0)
                    ll_other = get_linklist0(selectedNeighbors[idx]);
                else
                    ll_other = get_linklist(selectedNeighbors[idx], level);
                size_t sz_link_list_other = *ll_other;


                if (sz_link_list_other > Mcurmax)
                    throw std::runtime_error("Bad value of sz_link_list_other");
                if (selectedNeighbors[idx] == cur_c)
                    throw std::runtime_error("Trying to connect an element to itself");
                if (level > element_levels_[selectedNeighbors[idx]])
                    throw std::runtime_error("Trying to make a link on a non-existent level");

                tableint *data = (tableint *) (ll_other + 1);
                if (sz_link_list_other < Mcurmax) {
                    data[sz_link_list_other] = cur_c;
                    *ll_other = sz_link_list_other + 1;
                } else {
                    // finding the "weakest" element to replace it with the new one
                    dist_t d_max = fstdistfunc_(getDataByInternalId(cur_c), getDataByInternalId(selectedNeighbors[idx]),
                                                dist_func_param_);
                    // Heuristic:
                    std::priority_queue<std::pair<dist_t, tableint>, std::vector<std::pair<dist_t, tableint>>, CompareByFirst> candidates;
                    candidates.emplace(d_max, cur_c);

                    for (size_t j = 0; j < sz_link_list_other; j++) {
                        candidates.emplace(
                                fstdistfunc_(getDataByInternalId(data[j]), getDataByInternalId(selectedNeighbors[idx]),
                                             dist_func_param_), data[j]);
                    }

                    getNeighborsByHeuristic2(candidates, Mcurmax);

                    int indx = 0;
                    while (candidates.size() > 0) {
                        data[indx] = candidates.top().second;
                        candidates.pop();
                        indx++;
                    }
                    *ll_other = indx;
                    // Nearest K:
                    /*int indx = -1;
                    for (int j = 0; j < sz_link_list_other; j++) {
                        dist_t d = fstdistfunc_(getDataByInternalId(data[j]), getDataByInternalId(rez[idx]), dist_func_param_);
                        if (d > d_max) {
                            indx = j;
                            d_max = d;
                        }
                    }
                    if (indx >= 0) {
                        data[indx] = cur_c;
                    } */
                }

            }
        }

        std::mutex global;
        size_t ef_;

        void setEf(size_t ef) {
            ef_ = ef;
        }

        std::priority_queue<std::pair<dist_t, tableint>> searchKnnInternal(void *query_data, int k) {
            tableint currObj = enterpoint_node_;
            dist_t curdist = fstdistfunc_(query_data, getDataByInternalId(enterpoint_node_), dist_func_param_);

            for (size_t level = maxlevel_; level > 0; level--) {
                bool changed = true;
                while (changed) {
                    changed = false;
                    int *data;
                    data = (int *) (linkLists_[currObj] + (level - 1) * size_links_per_element_);
                    int size = *data;
                    tableint *datal = (tableint *) (data + 1);
                    for (int i = 0; i < size; i++) {
                        tableint cand = datal[i];
                        if (cand < 0 || cand > max_elements_)
                            throw std::runtime_error("cand error");
                        dist_t d = fstdistfunc_(query_data, getDataByInternalId(cand), dist_func_param_);

                        if (d < curdist) {
                            curdist = d;
                            currObj = cand;
                            changed = true;
                        }
                    }
                }
            }

            //std::priority_queue< std::pair< dist_t, tableint  >> top_candidates = searchBaseLayer(currObj, query_data, 0);
            std::priority_queue<std::pair<dist_t, tableint  >> top_candidates = searchBaseLayerST(currObj, query_data,
                                                                                                  ef_);
            while (top_candidates.size() > k) {
                top_candidates.pop();
            }
            return top_candidates;
        };


        void saveIndex(const std::string &location, const list<string> &ignore_list) {
            std::ofstream output(location, std::ios::binary);
            std::streampos position;

            writeBinaryPOD(output, offsetLevel0_);
            writeBinaryPOD(output, max_elements_);
            writeBinaryPOD(output, cur_element_count_);
            writeBinaryPOD(output, size_data_per_element_);    // =152
            writeBinaryPOD(output, label_offset_);    // 这个是什么 = 148
            writeBinaryPOD(output, offsetData_);    // = 132
            writeBinaryPOD(output, maxlevel_);
            writeBinaryPOD(output, enterpoint_node_);
            writeBinaryPOD(output, maxM_);

            writeBinaryPOD(output, maxM0_);
            writeBinaryPOD(output, M_);
            writeBinaryPOD(output, mult_);
            writeBinaryPOD(output, ef_construction_);

            writeBinaryPOD(output, normalize_);    // fj add
            ignore_word_size_ = (int)ignore_list.size();
            writeBinaryPOD(output, ignore_word_size_);    // 被忽略的词语的数量
            writeBinaryPOD(output, placeholder_0_);
            writeBinaryPOD(output, placeholder_1_);
            writeBinaryPOD(output, placeholder_2_);
            writeBinaryPOD(output, placeholder_3_);
            writeBinaryPOD(output, placeholder_4_);
            writeBinaryPOD(output, placeholder_5_);
            writeBinaryPOD(output, placeholder_6_);
            writeBinaryPOD(output, placeholder_7_);

            writeBinaryPOD(output, per_index_size_);
            output.write(index_ptr_, cur_element_count_ * per_index_size_);    // 写的时候，是cur_element_count_的信息

            if (ignore_list.size() > 0) {
                ignore_info_ = (char *)malloc(ignore_word_size_ * per_index_size_);
                memset(ignore_info_, 0, ignore_word_size_ * per_index_size_);
                int cur_num = 0;
                for (auto &cur : ignore_list) {
                    memcpy(ignore_info_ + cur_num * per_index_size_, cur.c_str(), per_index_size_);
                    cur_num++;
                }
                output.write(ignore_info_, ignore_word_size_ * per_index_size_);
            }

            output.write(data_level0_memory_, cur_element_count_ * size_data_per_element_);
            for (size_t i = 0; i < cur_element_count_; i++) {
                unsigned int linkListSize = element_levels_[i] > 0 ? size_links_per_element_ * element_levels_[i] : 0;
                writeBinaryPOD(output, linkListSize);
                if (linkListSize)
                    output.write(linkLists_[i], linkListSize);
            }

            output.close();
        }

        void loadIndex(const std::string &location, SpaceInterface<dist_t> *s, TrieProc* trie, size_t max_elements_i=0) {
            std::ifstream input(location, std::ios::binary);

            // get file size:
            input.seekg(0,input.end);
            std::streampos total_filesize=input.tellg();
            input.seekg(0,input.beg);

            readBinaryPOD(input, offsetLevel0_);
            readBinaryPOD(input, max_elements_);
            readBinaryPOD(input, cur_element_count_);

            size_t max_elements=max_elements_i;    // 针对默认传入的max_elements_i = 0 的情况
            if(max_elements < cur_element_count_)
                max_elements = max_elements_;
            max_elements_ = max_elements;
            readBinaryPOD(input, size_data_per_element_);
            readBinaryPOD(input, label_offset_);     // label的偏移量
            readBinaryPOD(input, offsetData_);    // 这里是260，表示数据的偏移量
            readBinaryPOD(input, maxlevel_);
            readBinaryPOD(input, enterpoint_node_);

            readBinaryPOD(input, maxM_);    // 16
            readBinaryPOD(input, maxM0_);    // 32
            readBinaryPOD(input, M_);
            readBinaryPOD(input, mult_);
            readBinaryPOD(input, ef_construction_);

            readBinaryPOD(input, normalize_);
            readBinaryPOD(input, ignore_word_size_);    // 读取被忽略词语的数量

            readBinaryPOD(input, placeholder_0_);
            readBinaryPOD(input, placeholder_1_);
            readBinaryPOD(input, placeholder_2_);
            readBinaryPOD(input, placeholder_3_);
            readBinaryPOD(input, placeholder_4_);
            readBinaryPOD(input, placeholder_5_);
            readBinaryPOD(input, placeholder_6_);
            readBinaryPOD(input, placeholder_7_);

            readBinaryPOD(input, per_index_size_);    // 每个单词最大size

            // 记住，这里是分配了max个信息，读取了cur的个数的信息
            index_ptr_ = (char *)malloc(max_elements_ * per_index_size_);
            memset(index_ptr_, 0, max_elements_ * per_index_size_);

            // 获取每个词语的最大长度
            input.read(index_ptr_,  cur_element_count_ * per_index_size_);
            char *word = (char *)malloc(per_index_size_);
            memset(word, 0, per_index_size_);
            for (int i = 0; i < cur_element_count_; ++i) {
                memcpy(word, index_ptr_ + i * per_index_size_, per_index_size_);
                index_lookup_.insert(BOOST_BIMAP::value_type(i, std::string(word)));
            }
            free(word);

            // 这里是load接口，ignore_info_刚开始肯定为nullptr
            ignore_info_ = (char *)malloc(ignore_word_size_ * per_index_size_);
            memset(ignore_info_, 0, per_index_size_ * ignore_word_size_);
            input.read(ignore_info_, ignore_word_size_ * per_index_size_);

            char *ignore_word = (char *)malloc(per_index_size_);
            memset(ignore_word, 0, per_index_size_);
            for (int i = 0; i < ignore_word_size_; i++) {
                memset(ignore_word, 0, per_index_size_);
                memcpy(ignore_word, ignore_info_ + i * per_index_size_, per_index_size_);
                trie->insert(std::string(ignore_word));
            }
            free(ignore_word);

            data_size_ = s->get_data_size();    // 这个是纯数据的大小，dim*size
            fstdistfunc_ = s->get_dist_func();
            dist_func_param_ = s->get_dist_func_param();

            /// Legacy, check that everything is ok

            bool old_index = false;

            auto pos=input.tellg();
            input.seekg(cur_element_count_ * size_data_per_element_, input.cur);
            for (size_t i = 0; i < cur_element_count_; i++) {
                int cur_size = (int)input.tellg();   // 它返回当前定位指针的位置，也代表着输入流的大小。
                if(cur_size < 0 || cur_size >= total_filesize){
                    old_index = true;
                    break;
                }

                unsigned int linkListSize;
                readBinaryPOD(input, linkListSize);
                if (linkListSize != 0) {
                    input.seekg(linkListSize, input.cur);
                }
            }

            // check if file is ok, if not this is either corrupted or old index
            if(input.tellg() != total_filesize)
                old_index = true;

            if (old_index) {
                std::cerr << "Warning: loading of old indexes will be deprecated before 2019.\n"
                          << "Please resave the index in the new format.\n";
            }
            input.clear();
            input.seekg(pos,input.beg);

            data_level0_memory_ = (char *) malloc(max_elements * size_data_per_element_);    // data_level0_memory_ 第0层总的buf的大小
            input.read(data_level0_memory_, cur_element_count_ * size_data_per_element_);

            if(old_index)
                input.seekg(((max_elements_ - cur_element_count_) * size_data_per_element_), input.cur);

            size_links_per_element_ = maxM_ * sizeof(tableint) + sizeof(linklistsizeint);    // maxM_是邻居个数，保存邻居节点，所需的最大长度，sizeof(tableint)和sizeof(linklistsizeint)都是4
            size_links_level0_ = maxM0_ * sizeof(tableint) + sizeof(linklistsizeint);
            std::vector<std::mutex>(max_elements).swap(link_list_locks_);

            visited_list_pool_ = new VisitedListPool(1, max_elements);

            linkLists_ = (char **) malloc(sizeof(void *) * max_elements);    // 这个是跳表的数据
            element_levels_ = std::vector<int>(max_elements);
            revSize_ = 1.0 / mult_;
            //ef_ = 10;
            ef_ = ef_construction_;    // 查询的时候，默认使用
            for (size_t i = 0; i < cur_element_count_; i++) {
                label_lookup_[getExternalLabel(i)]=i;
                unsigned int linkListSize;
                readBinaryPOD(input, linkListSize);
                if (linkListSize == 0) {
                    element_levels_[i] = 0;    // 就说明i这个节点，没有跳表数据
                    linkLists_[i] = nullptr;
                } else {
                    element_levels_[i] = linkListSize / size_links_per_element_;    // element_levels_[i]是第i个元素有多少层
                    linkLists_[i] = (char *) malloc(linkListSize);
                    input.read(linkLists_[i], linkListSize);    // 如果有信息的话，读入linkListSize个内容
                }
            }

            input.close();

            return;
        }

        template<typename data_t>
        std::vector<data_t> getDataByLabel(labeltype label)
        {
          tableint label_c;
          auto search = label_lookup_.find(label);
          if (search == label_lookup_.end()) {
              throw std::runtime_error("Label not found");
          }
          label_c = search->second;

          char* data_ptrv = getDataByInternalId(label_c);
          unsigned int dim = *((unsigned int *) dist_func_param_);    // 不转成int，会出偶现的问题
          std::vector<data_t> data;
          data_t* data_ptr = (data_t*) data_ptrv;
          for (unsigned int i = 0; i < dim; i++) {
            data.push_back(*data_ptr);
            data_ptr += 1;
          }
          return data;
        }

        int overwriteNode(void *node, const char *index) {
            // 重新写入node信息
            if (nullptr == node || nullptr == index) {
                return -2;
            }

            unsigned int len = (unsigned int)strlen(index);
            if (len > per_index_size_) {
                return -10;
            }

            labeltype label = (labeltype)index_lookup_.right.find(index)->second;

            char *buff = this->getDataByInternalId(label);    // 这里的label传入的值，不会超过real_count的大小
            memset(buff, 0, this->data_size_);
            memcpy(buff, node, this->data_size_);    // 更新node的内容

            memset(this->index_ptr_ + label * per_index_size_, 0, per_index_size_);
            memcpy(this->index_ptr_ + label * per_index_size_, index, len);    // 更新index对应的内容

            index_lookup_.left.erase(label);
            index_lookup_.insert(BOOST_BIMAP::value_type(label, std::string(index)));

            return 0;
        }

        /**
         * 根据传入的word信息，查询这个word对应的label（数字）是多少
         * @param word
         * @return
         */
        int findWordLabel(const char *word) {
            int label = -1;    // 默认，没找到就是返回-1
            auto result = index_lookup_.right.find(std::string(word));
            if (result != index_lookup_.right.end()) {
                label = (int)result->second;    // 如果查到了，就把对应的标签给出去
            }

            return label;
        }


        int addPoint(void *data_point, labeltype label, const char *index) {
           int ret = addPoint(data_point, label, index, -1);
           return ret;
        }

        int addPoint(void *data_point, labeltype label, const char* index, int level) {
            // 函数的ret值，是当前的个数
            if (index == nullptr || strlen(index) > per_index_size_) {
                return -10;
            }

            tableint cur_c = 0;
            {
                std::unique_lock <std::mutex> lock(cur_element_count_guard_);
                if (cur_element_count_ >= max_elements_) {
                    return -9;    // 有超过最大限制的话，就返回-9
                };
                cur_c = cur_element_count_;    // 如果当前是0，则保存

                memset(index_ptr_ + cur_element_count_ * per_index_size_, 0, per_index_size_);
                // add的时候，添加的内容，需要双向添加<label,index>，例子：<1, hello>
                memcpy(index_ptr_ + cur_element_count_ * per_index_size_, index, strlen(index));
                index_lookup_.insert(BOOST_BIMAP::value_type(label, std::string(index)));

                label_lookup_[label] = cur_c;  // expected unique, if not will overwrite
                cur_element_count_++;
            }

            std::unique_lock <std::mutex> lock_el(link_list_locks_[cur_c]);
            int curlevel = getRandomLevel(mult_);
            if (level > 0)
                curlevel = level;

            element_levels_[cur_c] = curlevel;
            std::unique_lock <std::mutex> templock(global);
            int maxlevelcopy = maxlevel_;
            if (curlevel <= maxlevelcopy)
                templock.unlock();
            tableint currObj = enterpoint_node_;

            memset(data_level0_memory_ + cur_c * size_data_per_element_ + offsetLevel0_, 0, size_data_per_element_);
            // Initialisation of the data and label
            memcpy(getExternalLabeLp(cur_c), &label, sizeof(labeltype));
            memcpy(getDataByInternalId(cur_c), data_point, data_size_);

            if (curlevel) {
                linkLists_[cur_c] = (char *) malloc(size_links_per_element_ * curlevel + 1);
                memset(linkLists_[cur_c], 0, size_links_per_element_ * curlevel + 1);
            }
            if ((signed)currObj != -1) {
                if (curlevel < maxlevelcopy) {
                    dist_t curdist = fstdistfunc_(data_point, getDataByInternalId(currObj), dist_func_param_);
                    for (int level = maxlevelcopy; level > curlevel; level--) {
                        bool changed = true;
                        while (changed) {
                            changed = false;
                            int *data;
                            std::unique_lock <std::mutex> lock(link_list_locks_[currObj]);
                            data = (int *) (linkLists_[currObj] + (level - 1) * size_links_per_element_);
                            int size = *data;
                            tableint *datal = (tableint *) (data + 1);
                            for (int i = 0; i < size; i++) {
                                tableint cand = datal[i];
                                if (cand < 0 || cand > max_elements_)
                                    return -9;
                                dist_t d = fstdistfunc_(data_point, getDataByInternalId(cand), dist_func_param_);
                                if (d < curdist) {
                                    curdist = d;
                                    currObj = cand;
                                    changed = true;
                                }
                            }
                        }
                    }
                }

                for (int level = std::min(curlevel, maxlevelcopy); level >= 0; level--) {
                    if (level > maxlevelcopy || level < 0) {
                        return -9;
                    }
                    std::priority_queue<std::pair<dist_t, tableint>, std::vector<std::pair<dist_t, tableint>>, CompareByFirst> top_candidates = searchBaseLayer(
                            currObj, data_point, level);
                    mutuallyConnectNewElement(data_point, cur_c, top_candidates, level);
                }
            } else {
                // Do nothing for the first element
                enterpoint_node_ = 0;
                maxlevel_ = curlevel;
            }

            //Releasing lock for the maximum level
            if (curlevel > maxlevelcopy) {
                enterpoint_node_ = cur_c;
                maxlevel_ = curlevel;
            }

            return 0;    // 0表示插入成功
            //return cur_c;
        };

        std::priority_queue<std::pair<dist_t, labeltype>> searchKnn(const void *query_data, size_t k) const {
            tableint currObj = enterpoint_node_;    // 进入点，是一个随机值，相当于最上层的入口点
            dist_t curdist = fstdistfunc_(query_data, getDataByInternalId(enterpoint_node_), dist_func_param_);    // 计算入口点和查询点的距离

            for (int level = maxlevel_; level > 0; level--) {
                bool changed = true;
                while (changed) {
                    changed = false;
                    int *data;
                    data = (int *) (linkLists_[currObj] + (level - 1) * size_links_per_element_);
                    int size = *data;    // 这个size表示，currObj在当前level，有size个邻居
                    tableint *datal = (tableint *) (data + 1);    // 其中，*data的[0]表示有多少个邻居，后面的数字，表示具体的邻居的index信息。
                    for (int i = 0; i < size; i++) {    // 在level层中查到currObj的最邻近的点
                        tableint cand = datal[i];
                        if (cand < 0 || cand > max_elements_)
                            throw std::runtime_error("cand error");
                        dist_t d = fstdistfunc_(query_data, getDataByInternalId(cand), dist_func_param_);

                        if (d < curdist) {
                            curdist = d;
                            currObj = cand;    // 传下去的currObj就相当于是
                            changed = true;
                        }
                    }
                }
            }

            std::priority_queue<std::pair<dist_t, tableint>, std::vector<std::pair<dist_t, tableint>>, CompareByFirst> top_candidates = searchBaseLayerST(
                    currObj, query_data, std::max(ef_,k));    // 在最低层查询信息
            std::priority_queue<std::pair<dist_t, labeltype> > results;
            while (top_candidates.size() > k) {    // 这里的top_candidates已经是最近的ef—search个节点了，但是只需要找k个点，所以把不需要的给pop掉
                top_candidates.pop();
            }
            while (top_candidates.size() > 0) {
                std::pair<dist_t, tableint> rez = top_candidates.top();
                results.push(std::pair<dist_t, labeltype>(rez.first, getExternalLabel(rez.second)));    // rez.first是距离信息，rez.second是index对应的信息，这里，index和label相同。我们通过label去找word信息
                top_candidates.pop();
            }
            return results;
        };


        std::priority_queue<std::pair<dist_t, labeltype>> forceLoop(const void *query_data, size_t topK) {
            // 暴力查找最近的topK个信息
            std::priority_queue<std::pair<dist_t, labeltype>> results;
            for (unsigned int i = 0; i < cur_element_count_; ++i) {
                float dist = fstdistfunc_(query_data, getDataByInternalId(i), dist_func_param_);
                results.push(std::pair<dist_t, labeltype>(dist, i));
                if (results.size() > topK) {
                    results.pop();
                }
            }

            return results;
        }


    };

}
