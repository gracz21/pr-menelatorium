#ifndef BUM_H
#define BUM_H

#include <queue>

#include "Request.h"
#include "Parameters.h"

class Bum {
    private:
        int id;
        int time;
        unsigned short weight;
        Parameters param;
        std::priority_queue<Request> requests;

        void hangAround();
    
    public:
        Bum(int id, Parameters param, int time = 0);
        int getId();
        void run();
};

#endif
