#ifndef BUM_H
#define BUM_H

#include <set>

#include "Request.h"
#include "HelpRequest.h"
#include "Parameters.h"

class Bum {
    private:
        int id;
        unsigned short weight;
        int time;
        const Parameters *worldParameters;
        std::set<Request> requests;
        std::set<HelpRequest> helpRequests;

        void hangAround();
        void checkForIncommingMessages();
        void goToMuseum();
        void handleMessageWhenIdle();
        void participateInExposition();
        void callForHelp();
        void leaveMuseum();
    
    public:
        Bum(int id, unsigned short weight, const Parameters *worldParameters,  int time = 0);
        int getId();
        void run();
};

#endif
