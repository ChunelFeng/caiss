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

    virtual ~UtilsProc() {
    }

    virtual CAISS_RET_TYPE init() {
        return 0;
    }
    virtual CAISS_RET_TYPE deinit() {
        return 0;
    }
};


#endif //CHUNELCAISS_UTILSPROC_H
