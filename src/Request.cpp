#include "../inc/Request.h"

Request::Request() {
    processId = -1;
    timestamp = -1;
    currentTime = -1;
}

Request::Request(int processId, int timestamp, int currentTime) {
    this->processId = processId;
    this->timestamp = timestamp;
    this->currentTime = currentTime;
}

bool Request::operator<(const Request &rhs) const {
    return (timestamp < rhs.timestamp) 
        || (timestamp == rhs.timestamp && processId < rhs.processId);
}
