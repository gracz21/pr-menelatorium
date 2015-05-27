#ifndef BUM_H
#define BUM_H

#include <queue>

#include "Request.h"
#include "HelpRequest.h"
#include "Parameters.h"

class Bum {
    private:
        int id;
        unsigned short weight;
        int time;
        const Parameters *worldParameters;
        std::priority_queue<Request> requests;
        std::priority_queue<HelpRequest> helpRequests;

        void hangAround();
        void checkForIncommingMessages();
        void goToMuseum();
        void handleMessageWhenIdle();
    
    public:
        Bum(int id, unsigned short weight, const Parameters *worldParameters,  int time = 0);
        int getId();
        void run();
};

#endif
