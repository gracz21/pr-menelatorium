#ifndef BUM_STATE_H
#define BUM_STATE_H

#include "Bum.h"

class BumState {
    protected: 
        Bum *context;

    public:
        BumState(Bum *context);
};

#endif
