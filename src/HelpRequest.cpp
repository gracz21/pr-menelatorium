#include "../inc/HelpRequest.h"

HelpRequest::HelpRequest() {
    processId = -1;
    timestamp = -1;
    currentTime = -1;
    weight = -1;
}

HelpRequest::HelpRequest(int processId, int timestamp, int currentTime, int weight) {
    this->processId = processId;
    this->timestamp = timestamp;
    this->currentTime = currentTime;
    this->weight = weight;
}

bool HelpRequest::operator<(const HelpRequest &rhs) const {
    return (timestamp < rhs.timestamp) 
        || (timestamp == rhs.timestamp && processId < rhs.processId);
}
