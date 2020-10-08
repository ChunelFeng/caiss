#pragma once
#include "hnswlib.h"

namespace hnswlib {

    // 欧式距离
    static float
    L2Sqr(const void *pVect1, const void *pVect2, const void *qty_ptr) {
        size_t qty = *((size_t *) qty_ptr);
        float res = 0;
        for (unsigned i = 0; i < qty; i++) {
            float t = ((float *) pVect1)[i] - ((float *) pVect2)[i];
            res += t * t;
        }
        return (res);

    }

    class L2Space : public SpaceInterface<float> {

        DISTFUNC<float> fstdistfunc_;
        size_t data_size_;
        size_t dim_;
    public:
        L2Space(size_t dim) {
            fstdistfunc_ = L2Sqr;
            dim_ = dim;
            data_size_ = dim * sizeof(float);
        }

        void set_dist_func(DISTFUNC<float> dist_func) {
            return;    // 具体距离，无任何操作
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

        ~L2Space() {}
    };

    static int
    L2SqrI(const void *__restrict pVect1, const void *__restrict pVect2, const void *__restrict qty_ptr) {

        size_t qty = *((size_t *) qty_ptr);
        int res = 0;
        unsigned char *a = (unsigned char *) pVect1;
        unsigned char *b = (unsigned char *) pVect2;
        /*for (int i = 0; i < qty; i++) {
            int t = int((a)[i]) - int((b)[i]);
            res += t*t;
        }*/

        qty = qty >> 2;
        for (size_t i = 0; i < qty; i++) {

            res += ((*a) - (*b)) * ((*a) - (*b));
            a++;
            b++;
            res += ((*a) - (*b)) * ((*a) - (*b));
            a++;
            b++;
            res += ((*a) - (*b)) * ((*a) - (*b));
            a++;
            b++;
            res += ((*a) - (*b)) * ((*a) - (*b));
            a++;
            b++;


        }

        return (res);

    }

    class L2SpaceI : public SpaceInterface<int> {

        DISTFUNC<int> fstdistfunc_;
        size_t data_size_;
        size_t dim_;
    public:
        L2SpaceI(size_t dim) {
            fstdistfunc_ = L2SqrI;
            dim_ = dim;
            data_size_ = dim * sizeof(unsigned char);
        }

        size_t get_data_size() {
            return data_size_;
        }

        DISTFUNC<int> get_dist_func() {
            return fstdistfunc_;
        }

        void *get_dist_func_param() {
            return &dim_;
        }

        ~L2SpaceI() {}
    };


}
