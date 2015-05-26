#ifndef BUM_H
#define BUM_H

#include <queue>

#include "Request.h"
#include "Parameters.h"

class Bum {
    private:
        int id;
        unsigned short weight;
        int time;
        const Parameters *worldParameters;
        std::priority_queue<Request> requests;

        void hangAround();
        void checkForIncommingMessages();
        void goToMuseum();
    
    public:
        Bum(int id, unsigned short weight, const Parameters *worldParameters,  int time = 0);
        int getId();
        void run();
};

#endif
