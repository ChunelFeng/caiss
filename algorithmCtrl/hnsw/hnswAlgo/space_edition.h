//
// Created by Chunel on 2020/6/12.
//

#ifndef CHUNELANN_SPACE_EDITION_H
#define CHUNELANN_SPACE_EDITION_H

#pragma once
#include "hnswlib.h"

namespace hnswlib {
    class EditionProductSpace : public SpaceInterface<float> {

        DISTFUNC<float> fstdistfunc_;
        size_t data_size_;
        size_t dim_;

    public:
        EditionProductSpace(size_t dim) {
            fstdistfunc_ = nullptr;
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

        void set_dist_func(DISTFUNC<float> func) {
            // 在定制距离的情况下，会使用set
            if (nullptr != func) {
                this->fstdistfunc_ = func;
            }
        }

        ~EditionProductSpace() {}
    };


}


#endif //CHUNELANN_SPACE_EDITION_H
