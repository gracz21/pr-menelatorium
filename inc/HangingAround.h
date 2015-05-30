#ifndef HANGING_AROUND_H
#define HANGING_AROUND_H

#include "BumState.h"

class Bum;

class HangingAround: public BumState {
    public:
        HangingAround(Bum *context);

        void handleEnterReq(MPI_Status &status);
        void handleExitNotification(MPI_Status &status);
        void handleEnterResp(MPI_Status &status);
        void handleExpoStart(MPI_Status &status);
        void handleExpoStartConfirmation(MPI_Status &status);
        void handleHelpReq(MPI_Status &status);
        void handleHelpResp(MPI_Status &status);
        void handleNurseReleaseNotification(MPI_Status &status);
};

#endif
