/***************************
@Author: Chunel
@Contact: chunel@foxmail.com
@File: SingletonProc.h
@Time: 2020/12/9 8:55 下午
@Desc: 模板类
***************************/

#ifndef CAISS_SINGLETON_H
#define CAISS_SINGLETON_H

#include "../UtilsProc.h"
#include "../../threadCtrl/ThreadInclude.h"


template<typename T>
class Singleton : public UtilsProc {
public:
    explicit Singleton() {
        handle_ = nullptr;
    }

    ~Singleton() override {
        CAISS_DELETE_PTR(handle_)
    }

    CAISS_STATUS create() {
        CAISS_FUNCTION_BEGIN
        if (nullptr == handle_) {
            lock_.writeLock();
            if (nullptr == handle_) {
                handle_ = new T();
            }
            lock_.writeUnlock();
        } else {
            ret = CAISS_RET_WARNING;
        }

        CAISS_FUNCTION_CHECK_STATUS
        CAISS_FUNCTION_END
    }

    // 这个是专门为了mrpt模型抽出来的一个create函数
    CAISS_STATUS create(const float *data, int dim, int samples) {
        CAISS_FUNCTION_BEGIN
        if (nullptr == handle_) {
            CAISS_ASSERT_NOT_NULL(data)    // 延迟校验，避免重复初始化的时候抛出异常信息
            lock_.writeLock();
            if (nullptr == handle_) {
                handle_ = new T(data, dim, samples);
            }
            lock_.writeUnlock();
        } else {
            ret = CAISS_RET_WARNING;
        }

        CAISS_FUNCTION_CHECK_STATUS
        CAISS_FUNCTION_END
    }

    T* get() {
        return handle_;
    }

    CAISS_STATUS destroy() {
        CAISS_FUNCTION_BEGIN
        lock_.writeLock();
        CAISS_DELETE_PTR(handle_)
        lock_.writeUnlock();
        CAISS_FUNCTION_END
    }

private:
    T* handle_;
    RWLock lock_;
};


#endif //CAISS_SINGLETON_H
