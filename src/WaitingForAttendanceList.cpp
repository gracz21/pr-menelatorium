#include "../inc/WaitingForAttendanceList.h"
#include "../inc/Bum.h"
#include "../inc/MPIRequest.h"
#include "../inc/Request.h"

#include <mpi.h>

WaitingForAttendanceList::WaitingForAttendanceList(Bum *context): BumState(context) {}

void WaitingForAttendanceList::handleEnterReq(MPI_Status &status) {
    context->answerWantToEnterMuseum(status);
}

void WaitingForAttendanceList::handleExitNotification(MPI_Status &status) {
    printf("Unexpected exit notification when waiting for attendance list\n");
    throw "";
}

void WaitingForAttendanceList::handleEnterResp(MPI_Status &status) {
    printf("Unexpected enter response when waiting for attendance list\n");
    throw "";
}

void WaitingForAttendanceList::handleExpoStart(MPI_Status &status) {
    context->saveMuseumAttendanceList(status);
}

void WaitingForAttendanceList::handleExpoStartConfirmation(MPI_Status &status) {
    printf("Unexpected expo start when waiting for attendance list\n");
    throw "";
}

void WaitingForAttendanceList::handleHelpReq(MPI_Status &status) {
    context->answerDontNeedNurse(status);
}

void WaitingForAttendanceList::handleHelpResp(MPI_Status &status) {
    printf("Unexpected help response when waiting for attendance list\n");
    throw "";
}

void WaitingForAttendanceList::handleNurseReleaseNotification(MPI_Status &status) {
    printf("Unexpected nurse release notification when waiting for attendance list\n");
    throw "";
}

void WaitingForAttendanceList::handleMuseumLock(MPI_Status &status) {
    context->saveMuseumLock(status);
}

void WaitingForAttendanceList::handleSingleExit(MPI_Status &status) {
    printf("Unexpected single exit when waiting for attendance list\n");
    throw "Unexpected message";
}
