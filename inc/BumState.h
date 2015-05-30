#ifndef BUM_STATE_H
#define BUM_STATE_H

#include <mpi.h>

class Bum;

class BumState {
    protected: 
        Bum *context;
        virtual void handleEnterReq(MPI_Status &status) = 0;
        virtual void handleExitNotification(MPI_Status &status) = 0;
        virtual void handleEnterResp(MPI_Status &status) = 0;
        virtual void handleExpoStart(MPI_Status &status) = 0;
        virtual void handleExpoStartConfirmation(MPI_Status &status) = 0;
        virtual void handleHelpReq(MPI_Status &status) = 0;
        virtual void handleHelpResp(MPI_Status &status) = 0;
        virtual void handleNurseReleaseNotification(MPI_Status &status) = 0;

    public:
        BumState(Bum *context);
        void handleMessage(MPI_Status &status);
        virtual ~BumState();
};

#endif
