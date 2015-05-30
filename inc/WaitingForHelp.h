#ifndef WAITING_FOR_HELP_H
#define WAITING_FOR_HELP_H

#include "BumState.h"

class Bum;

class WaitingForHelp: public BumState {
    protected:
        void handleEnterReq(MPI_Status &status);
        void handleExitNotification(MPI_Status &status);
        void handleEnterResp(MPI_Status &status);
        void handleExpoStart(MPI_Status &status);
        void handleExpoStartConfirmation(MPI_Status &status);
        void handleHelpReq(MPI_Status &status);
        void handleHelpResp(MPI_Status &status);
        void handleNurseReleaseNotification(MPI_Status &status);

    public:
        WaitingForHelp(Bum *context);
};

#endif
