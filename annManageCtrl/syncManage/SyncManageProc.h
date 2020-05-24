//
// Created by Chunel on 2020/5/24.
//

#ifndef CHUNELANN_SYNCMANAGEPROC_H
#define CHUNELANN_SYNCMANAGEPROC_H

#include "../AnnManageProc.h"

class SyncManageProc : public AnnManageProc  {
public:
    SyncManageProc(unsigned int maxSize);


    ANN_RET_TYPE createHandle(void **handle);    // 生成处理句柄
    ANN_RET_TYPE destroyHandle(void *handle);

};


#endif //CHUNELANN_SYNCMANAGEPROC_H
