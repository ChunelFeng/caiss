//
// Created by Chunel on 2020/5/24.
//

#ifndef CHUNELANN_SYNCMANAGEPROC_H
#define CHUNELANN_SYNCMANAGEPROC_H

#include "../AnnManageProc.h"

class SyncManageProc : public AnnManageProc  {
public:
    explicit SyncManageProc(unsigned int maxSize);
    ~SyncManageProc() override;

    ANN_RET_TYPE createHandle(void **handle) override;    // 生成处理句柄
    ANN_RET_TYPE destroyHandle(void *handle) override;

    AlgorithmProc* getInstance(void *handle);

};


#endif //CHUNELANN_SYNCMANAGEPROC_H
