#include "../inc/BumState.h"
#include "../inc/messages.h"

BumState::BumState(Bum *context) {
    this->context = context;
}

BumState::~BumState() {

}

void BumState::handleMessage(MPI_Status &status) {
    switch (status.MPI_TAG) {
        case ENTER_REQ:
            this->handleEnterReq(status); 
            break;
        case EXIT_NOTIFICATION:
            this->handleExitNotification(status);
            break;
        case ENTER_RESP:
            this->handleEnterResp(status);
            break;
        case EXPO_START:
            this->handleExpoStart(status);
            break;
        case EXPO_START_CONFIRMATION:
            this->handleExpoStartConfirmation(status);
            break;
        case HELP_REQ:
            this->handleHelpReq(status);
            break;
        case HELP_RESP:
            this->handleHelpResp(status);
            break;
        case NURSE_RELEASE_NOTIFICATION:
            this->handleNurseReleaseNotification(status);
            break;
    }
}
