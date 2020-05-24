//
// Created by Chunel on 2020/5/24.
//

#include "SyncManageProc.h"

ANN_RET_TYPE SyncManageProc::createHandle(void** handle) {
    ANN_FUNCTION_BEGIN

    if (this->manages_.empty()) {
        return ANN_RET_RES;    // 如果是空，表示返回失败
    }

    this->lock_.writeLock();    // 如果内部还有句柄信息的话，开始分配句柄操作


    *handle = (void *)this->manages_.front();
    this->manages_.pop();    // 弹出第一个

    this.lock_.writeUnlock();
    ANN_FUNCTION_END
}

ANN_RET_TYPE SyncManageProc::destroyHandle(void* handle) {
    ANN_ASSERT_NOT_NULL(handle);

    ANN_FUNCTION_BEGIN



    this.lock_.writeLock();
    this->manages_.push(handle);    // todo 这里重点考虑啊。这样，不好删除句柄的。可以考虑用一个int值来跟handle配
    this.lock_.writeUnlock();

    ANN_FUNCTION_END
}

SyncManageProc::SyncManageProc(unsigned int maxSize) : AnnManageProc(maxSize) {
    for(unsigned int i = 0; i < maxSize; i++) {
        AlgorithmProc* pc = new AlgorithmProc();    // todo 需要想办法关联到具体的算法proc类
        this->manages_.push(pc);
    }
    this->max_size_ = maxSize;

}
