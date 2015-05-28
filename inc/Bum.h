#ifndef BUM_H
#define BUM_H

#include <set>
#include <list>

#include "Request.h"
#include "HelpRequest.h"
#include "Parameters.h"

class Bum {
    private:
        int id;
        unsigned short weight;
        int time;
        const Parameters *worldParameters;
        std::set<Request> enterRequests;
        std::set<HelpRequest> helpRequests;
        std::set<HelpRequest> helpRequestsFilter;
        std::list<Request> delayedEnterRequests;

        const HelpRequest *myHelpRequest;
        int *museumAttendanceList;

        ~Bum();
        void hangAround();
        void checkForIncommingMessages();
        void goToMuseum();
        void handleMessageWhenIdle();
        void participateInExposition();
        void callForHelp();
        void waitForHelp();
        void leaveMuseum();
    
    public:
        Bum(int id, unsigned short weight, const Parameters *worldParameters,  int time = 0);
        int getId();
        void run();
};

#endif
