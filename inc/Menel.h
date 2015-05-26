#ifndef MENEL_H
#define MENEL_H

#include <queue>

#include "Request.h"

class Menel {
    private:
        int id;
        int time;
        std::priority_queue<Request> requests;

        void hangAround();
    
    public:
        Menel(int id, int time = 0);
        int getId();
        void run();
};

#endif
