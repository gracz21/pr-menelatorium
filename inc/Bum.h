#ifndef BUM_H
#define BUM_H

#include <set>
#include <list>
#include <mpi.h>

#include "Request.h"
#include "HelpRequest.h"
#include "Parameters.h"

class BumState;

class Bum {
    friend class BumState;
    private:
        int id;
        unsigned short weight;
        int time;
        BumState* state;
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
        bool museumAttendanceListUpdated;

        void emptyDelayedEnterRequests();
        void hangAround();
        void checkForIncommingMessages();

        void goToMuseum();
        void sendEnterRequests();
        void waitForEnterResponses();
        bool tryToEnterMuseum();
        void waitForExpositionStart();
        void sendAttendanceList();
        void waitForAttendanceList();

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
        void addToEnterRequestsFilter(Request *enterRequests);
        void removeFromEnterRequests(Request *enterRequests);
    
    public:
        Bum(int id, unsigned short weight, const Parameters *worldParameters, int* bumsIds, int time = 0);
        ~Bum();
        void run();
};

#endif
