#include "../inc/WaitingForEnterResponses.h"
#include "../inc/Bum.h"
#include "../inc/MPIRequest.h"
#include "../inc/Request.h"

#include <mpi.h>

WaitingForEnterResponses::WaitingForEnterResponses(Bum *context): BumState(context) {}

void WaitingForEnterResponses::handleEnterReq(MPI_Status &status) {
    context->answerWantToEnterMuseum(status);
}

void WaitingForEnterResponses::handleExitNotification(MPI_Status &status) {
    context->saveExitNotification(status);
}

void WaitingForEnterResponses::handleEnterResp(MPI_Status &status) {
    context->saveEnterRequest(status);
}

void WaitingForEnterResponses::handleExpoStart(MPI_Status &status) {
    context->saveMuseumAttendanceList(status);
}

void WaitingForEnterResponses::handleExpoStartConfirmation(MPI_Status &status) {
    printf("Unexpected expo start confirmation when waiting for enter responses\n");
    throw "Unexpected expo start confirmation when waiting for enter responses";
}

void WaitingForEnterResponses::handleHelpReq(MPI_Status &status) {
    context->answerDontNeedNurse(status);
}

void WaitingForEnterResponses::handleHelpResp(MPI_Status &status) {
    printf("Unexpected help response when waiting for enter responses\n");
    throw "Unexpected expo start confirmation when waiting for enter responses";
}

void WaitingForEnterResponses::handleNurseReleaseNotification(MPI_Status &status) {
    context->ignoreNurseReleaseNotification(status);
}

void WaitingForEnterResponses::handleMuseumLock(MPI_Status &status) {
    context->saveMuseumLock(status);
}

void WaitingForEnterResponses::handleSingleExit(MPI_Status &status) {
    printf("Unexpected single exit when waiting for enter responses\n");
    throw "Unexpected message";
}
