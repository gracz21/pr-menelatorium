#include "../inc/Request.h"

Request::Request(int processId, int timestamp) {
    this->processId = processId;
    this->timestamp = timestamp;
}

bool Request::operator<(const Request &rhs) const {
    return (timestamp < rhs.timestamp) 
        || (timestamp == rhs.timestamp && processId < rhs.processId);
}
