#include "../inc/WaitingForHelp.h"
#include "../inc/Bum.h"
#include "../inc/MPIRequest.h"
#include "../inc/Request.h"

#include <mpi.h>

WaitingForHelp::WaitingForHelp(Bum *context): BumState(context) {}

void WaitingForHelp::handleEnterReq(MPI_Status &status) {
    context->answerWantToEnterMuseum(status);
}

void WaitingForHelp::handleExitNotification(MPI_Status &status) {
    printf("Unexpected exit notification whene waiting for help\n");
    throw "";
}

void WaitingForHelp::handleEnterResp(MPI_Status &status) {
    printf("Unexpected enter resp when waiting for help\n");
    throw "";
}

void WaitingForHelp::handleExpoStart(MPI_Status &status) {
    printf("Unexpected expo start when waiting for help\n");
    throw "";
}

void WaitingForHelp::handleExpoStartConfirmation(MPI_Status &status) {
    printf("Unexpected expo start confirmation when waiting for help\n");
    throw "";
}

void WaitingForHelp::handleHelpReq(MPI_Status &status) {
    context->saveHelpReq(status);
}

void WaitingForHelp::handleHelpResp(MPI_Status &status) {
    context->saveHelpResp(status);
}

void WaitingForHelp::handleNurseReleaseNotification(MPI_Status &status) {
    context->saveNurseRelease(status);
}

void WaitingForHelp::handleMuseumLock(MPI_Status &status) {
    printf("Unexpected museum lock when waiting for help\n");
    throw "";
}

void WaitingForHelp::handleSingleExit(MPI_Status &status) {
    if ((context->id == context->museumAttendanceList[context->worldParameters->s - 1])) {
        context->delayExitNotification(status);
    } else {
        printf("Unexpected single exit notification whene waiting for help\n");
        throw "";
    }
}
