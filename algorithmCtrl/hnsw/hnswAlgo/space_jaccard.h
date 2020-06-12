#pragma once
#include <set>
#include "hnswlib.h"

namespace hnswlib {

    static float
    JaccardProduct(const void *pVect1, const void *pVect2, const void *qty_ptr) {
        size_t qty = *((size_t *) qty_ptr);    // 相当于是dim信息
        unsigned int diff = 0;    // 交集
        unsigned int same = 0;    // 并集

        std::set<float> first_set;
        std::set<float> second_set;
        for (unsigned int i = 0; i < qty; i++) {
            first_set.insert(((float *) pVect1)[i]);
            second_set.insert(((float *) pVect2)[i]);
        }

        for (auto x : first_set) {
            if (second_set.find(x) == second_set.end()) {
                diff++;    // 第一个在第二个中没有的
            } else {
                same++;    // 第一个在第二个中包含的
            }
        }

        float res = (float)same / (float)(second_set.size() + diff);    // 相同的 除以 （第二个size + 第一个有 & 第二个没有的）
        return 1.0f - res;
    }


    class JaccardProductSpace : public SpaceInterface<float> {

        DISTFUNC<float> fstdistfunc_;
        size_t data_size_;
        size_t dim_;

    public:
        JaccardProductSpace(size_t dim) {
            fstdistfunc_ = JaccardProduct;
            dim_ = dim;
            data_size_ = dim * sizeof(float);
        }

        size_t get_data_size() {
            return data_size_;
        }

        DISTFUNC<float> get_dist_func() {
            return fstdistfunc_;
        }

        void *get_dist_func_param() {
            return &dim_;
        }

        ~JaccardProductSpace() {}
    };


}
