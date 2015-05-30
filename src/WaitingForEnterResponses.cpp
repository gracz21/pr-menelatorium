#include "../inc/WaitingForEnterResponses.h"
#include "../inc/Bum.h"
#include "../inc/MPIRequest.h"
#include "../inc/Request.h"

#include <mpi.h>

WaitingForEnterResponses::WaitingForEnterResponses(Bum *context): BumState(context) {}

void WaitingForEnterResponses::handleEnterReq(MPI_Status &status) {
    context->answerDontWantToEnterMuseum(status);
}

void WaitingForEnterResponses::handleExitNotification(MPI_Status &status) {
    context->ignoreExitNotifications(status);
}

void WaitingForEnterResponses::handleEnterResp(MPI_Status &status) {
    printf("Unexpected enter response when hanging around\n");
    throw "Unexpected message";
}

void WaitingForEnterResponses::handleExpoStart(MPI_Status &status) {
    printf("Unexpected exposition start notification when hanging around\n");
    throw "Unexpected message";
}

void WaitingForEnterResponses::handleExpoStartConfirmation(MPI_Status &status) {
    printf("Unexpected expo start confirmation when hanging around\n");
    throw "Unexpected message";
}

void WaitingForEnterResponses::handleHelpReq(MPI_Status &status) {
    context->answerDontNeedNurse(status);
}

void WaitingForEnterResponses::handleHelpResp(MPI_Status &status) {
    printf("Unexpected help response when hanging around\n");
    throw "Unexpected message";
}

void WaitingForEnterResponses::handleNurseReleaseNotification(MPI_Status &status) {
    context->ignoreNurseReleaseNotification(status);
}
