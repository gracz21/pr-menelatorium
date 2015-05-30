#include "../inc/WaitingForExit.h"
#include "../inc/Bum.h"
#include "../inc/MPIRequest.h"
#include "../inc/Request.h"

#include <mpi.h>

WaitingForExit::WaitingForExit(Bum *context): BumState(context) {}

void WaitingForExit::handleEnterReq(MPI_Status &status) {
    context->answerWantToEnterMuseum(status);
}

void WaitingForExit::handleExitNotification(MPI_Status &status) {
    context->delayExitNotification(status);
}

void WaitingForExit::handleEnterResp(MPI_Status &status) {
    printf("Unexpected enter response when waiting for exit\n");
    throw "";
}

void WaitingForExit::handleExpoStart(MPI_Status &status) {
    printf("Unexpected expo start when waiting for exit\n");
    throw "";
}

void WaitingForExit::handleExpoStartConfirmation(MPI_Status &status) {
    printf("Unexpected expo start confirmation when waiting for exit\n");
    throw "";
}

void WaitingForExit::handleHelpReq(MPI_Status &status) {
    context->answerDontNeedNurse(status);
}

void WaitingForExit::handleHelpResp(MPI_Status &status) {
    printf("Unexpected help response when waiting for exit\n");
    throw "";
}

void WaitingForExit::handleNurseReleaseNotification(MPI_Status &status) {
    context->ignoreNurseReleaseNotification(status);
}

void WaitingForExit::handleMuseumLock(MPI_Status &status) {
    printf("Unexpected museum lock when waiting for exit\n");
    throw "";
}
