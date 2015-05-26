#ifndef BUM_H
#define BUM_H

#include <queue>

#include "Request.h"

class Bum {
    private:
        int id;
        int time;
        unsigned short weight;
        std::priority_queue<Request> requests;

        void hangAround();
    
    public:
        Bum(int id, int time = 0);
        int getId();
        void run();
};

#endif
