#include "../inc/HangingAround.h"
#include "../inc/Bum.h"

HangingAround::HangingAround(Bum *context): BumState(context) {}

void HangingAround::handleEnterReq(MPI_Status &status) {
}

void HangingAround::handleExitNotification(MPI_Status &status) {

}

void HangingAround::handleEnterResp(MPI_Status &status) {

}

void HangingAround::handleExpoStart(MPI_Status &status) {

}

void HangingAround::handleExpoStartConfirmation(MPI_Status &status) {

}

void HangingAround::handleHelpReq(MPI_Status &status) {

}

void HangingAround::handleHelpResp(MPI_Status &status) {

}

void HangingAround::handleNurseReleaseNotification(MPI_Status &status) {

}
