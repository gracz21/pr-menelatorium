#ifndef WAITING_FOR_ATTENDANCE_LIST_H
#define WAITING_FOR_ATTENDANCE_LIST_H

#include "BumState.h"

class Bum;

class WaitingForAttendanceList: public BumState {
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
        WaitingForAttendanceList(Bum *context);
};

#endif
