#ifndef BUM_H
#define BUM_H

#include <set>
#include <list>
#include <map>
#include <string>
#include <mpi.h>

#include "Request.h"
#include "HelpRequest.h"
#include "Parameters.h"

class BumState;

class Bum {
    friend class HangingAround;
    friend class WaitingForEnterResponses;
    friend class WaitingForAttendanceList;
    friend class WaitingForHelp;
    friend class WaitingForExit;

    private:
        int id;
        unsigned short weight;
        int time;
        std::map<std::string, BumState*> states;
        const Parameters *worldParameters;
        std::set<Request> enterRequests;
        std::set<Request> enterRequestsFilter;
        std::set<HelpRequest> helpRequests;
        std::set<HelpRequest> helpRequestsFilter;
        std::list<Request> exitNotifications;
        bool museumLocked;

        const HelpRequest *myHelpRequest;
        const Request *myEnterRequest;
        int *museumAttendanceList;
        int *bumsIds;
        bool museumAttendanceListReceived;

        void hangAround();

        void goToMuseum();
        void sendEnterRequests();
        void waitForEnterResponses();
        bool tryToEnterMuseum();
        void waitForExpositionStart();
        void sayHelloToMuseumService();
        void sendAttendanceList();
        void waitForLockConfirmations();
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
        void sayGoodbyeToMuseumService();
        void addToEnterRequestsFilter(Request *enterRequests);
        void removeFromEnterRequests(Request *enterRequests);

        void answerDontWantToEnterMuseum(MPI_Status &status);
        void answerWantToEnterMuseum(MPI_Status &status);
        void ignoreExitNotifications(MPI_Status &status);
        void answerDontNeedNurse(MPI_Status &status);
        void ignoreNurseReleaseNotification(MPI_Status &status);
        void saveEnterRequest(MPI_Status &status);
        void saveExitNotification(MPI_Status &status);
        void saveMuseumAttendanceList(MPI_Status &status);
        void saveHelpReq(MPI_Status &status);
        void saveHelpResp(MPI_Status &status);
        void saveNurseRelease(MPI_Status &status);
        void delayExitNotification(MPI_Status &status);
        void saveMuseumLock(MPI_Status &status);
    
    public:
        Bum(int id, unsigned short weight, const Parameters *worldParameters, int* bumsIds, int time = 0);
        ~Bum();
        void run();
};

#endif
