#pragma once
#ifndef NO_MANUAL_VECTORIZATION
// compatible with msvc https://stackoverflow.com/questions/18563978/detect-the-availability-of-sse-sse2-instruction-set-in-visual-studio
#if defined(__SSE2__) || (_MSC_VER && (defined(_M_AMD64) || defined(_M_X64) || _M_IX86_FP == 2 || _M_IX86_FP == 1))
#define USE_SSE
#ifdef __AVX__
#define USE_AVX
#endif
#endif
#endif

#if defined(USE_AVX) || defined(USE_SSE)
#ifdef _MSC_VER
#include <intrin.h>
#include <stdexcept>
#else
#include <x86intrin.h>
#endif

#if defined(__GNUC__)
#define PORTABLE_ALIGN32 __attribute__((aligned(32)))
#else
#define PORTABLE_ALIGN32 __declspec(align(32))
#endif
#endif


#include <iostream>
#include <queue>
#include <list>
#include <cstring>


namespace hnswlib {
    typedef size_t labeltype;

    template<typename T>
    static void writeBinaryPOD(std::ostream &out, const T &podRef) {
        out.write((char *) &podRef, sizeof(T));
    }

    template<typename T>
    static void readBinaryPOD(std::istream &in, T &podRef) {
        in.read((char *) &podRef, sizeof(T));
    }

    template<typename MTYPE>
    using DISTFUNC = MTYPE(*)(const void *, const void *, const void *);


    template<typename MTYPE>
    class SpaceInterface {
    public:
        //virtual void search(void *);
        virtual size_t get_data_size() = 0;

        virtual DISTFUNC<MTYPE> get_dist_func() = 0;

        virtual void set_dist_func(DISTFUNC<MTYPE> dist_func) = 0;

        virtual void *get_dist_func_param() = 0;

        virtual ~SpaceInterface() {}
    };

    template<typename dist_t>
    class AlgorithmInterface {
    public:
        virtual int addPoint(void *datapoint, labeltype label, const char *index)=0;
        virtual std::priority_queue<std::pair<dist_t, labeltype >> searchKnn(const void *, size_t) const = 0;
        virtual void saveIndex(const std::string &location, const std::list<std::string> &ignoreList)=0;
        virtual ~AlgorithmInterface(){
        }
    };
}

#include "space_l2.h"
#include "space_ip.h"
#include "space_jaccard.h"
#include "space_edition.h"
#include "bruteforce.h"
#include "hnswalg.h"
