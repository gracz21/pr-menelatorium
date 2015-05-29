#ifndef BUM_H
#define BUM_H

#include <set>
#include <list>
#include <mpi.h>

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
        std::set<Request> enterRequestsFilter;
        std::set<HelpRequest> helpRequests;
        std::set<HelpRequest> helpRequestsFilter;
        std::list<Request> delayedEnterRequests;
        std::list<Request> exitNotifications;

        const HelpRequest *myHelpRequest;
        const Request *myEnterRequest;
        int *museumAttendanceList;
        int *bumsIds;

        void emptyDelayedEnterRequests();
        void hangAround();
        void checkForIncommingMessages();

        void goToMuseum();
        void sendEnterRequests();
        void waitForEnterResponses();
        bool tryToEnterMuseum();
        void waitForExpositionStart();

        void handleMessageWhenIdle(MPI_Status &status);
        void participateInExposition();
        void callForHelp();
        void waitForHelp();
        bool tryToGetHelp();
        void releaseNurses();
        void insertHelpRequest(HelpRequest &helpRequest);
        void insertEnterRequest(Request &enterRequest);
        void leaveMuseum();
        void notifyAboutExit();
        void waitForOthersToExit();
    
    public:
        Bum(int id, unsigned short weight, const Parameters *worldParameters, int* bumsIds, int time = 0);
        ~Bum();
        int getId();
        void run();
};

#endif
