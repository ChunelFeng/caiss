//
// Created by Chunel on 2020/5/24.
//

#ifndef CHUNELANN_UTILSPROC_H
#define CHUNELANN_UTILSPROC_H

#include "../chunelAnnLib/ChunelAnnLibDefine.h"

class UtilsProc {
public:
    UtilsProc() {

    }
    virtual ANN_RET_TYPE init() = 0;
    virtual ANN_RET_TYPE deinit() = 0;

protected:
    virtual ~UtilsProc() {

    }
};


#endif //CHUNELANN_UTILSPROC_H
