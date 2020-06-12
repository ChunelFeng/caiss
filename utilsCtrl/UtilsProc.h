//
// Created by Chunel on 2020/5/24.
//

#ifndef CHUNELCAISS_UTILSPROC_H
#define CHUNELCAISS_UTILSPROC_H

#include "../caissLib/CaissLibDefine.h"

class UtilsProc {
public:
    explicit UtilsProc() {

    }
    virtual CAISS_RET_TYPE init() = 0;
    virtual CAISS_RET_TYPE deinit() = 0;

protected:
    virtual ~UtilsProc() {

    }
};


#endif //CHUNELCAISS_UTILSPROC_H
