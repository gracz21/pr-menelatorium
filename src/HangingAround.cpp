#include "../inc/HangingAround.h"
#include "../inc/Bum.h"
#include "../inc/MPIRequest.h"
#include "../inc/Request.h"

#include <mpi.h>

HangingAround::HangingAround(Bum *context): BumState(context) {}

void HangingAround::handleEnterReq(MPI_Status &status) {
    context->answerDontWantToEnterMuseum(status);
}

void HangingAround::handleExitNotification(MPI_Status &status) {
    context->ignoreExitNotifications(status);
}

void HangingAround::handleEnterResp(MPI_Status &status) {
    printf("Unexpected enter response when hanging around\n");
    throw "Unexpected message";
}

void HangingAround::handleExpoStart(MPI_Status &status) {
    printf("Unexpected exposition start notification when hanging around\n");
    throw "Unexpected message";
}

void HangingAround::handleExpoStartConfirmation(MPI_Status &status) {
    printf("Unexpected expo start confirmation when hanging around\n");
    throw "Unexpected message";
}

void HangingAround::handleHelpReq(MPI_Status &status) {
    context->answerDontNeedNurse(status);
}

void HangingAround::handleHelpResp(MPI_Status &status) {
    printf("Unexpected help response when hanging around\n");
    throw "Unexpected message";
}

void HangingAround::handleNurseReleaseNotification(MPI_Status &status) {
    context->ignoreNurseReleaseNotification(status);
}
