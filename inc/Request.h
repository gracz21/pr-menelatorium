#ifndef REQUEST_H
#define REQUEST_H

#include "Message.h"

struct Request: Message {
    int processId;
    int timestamp;

    Request(int processId, int bimestamp);
    bool operator<(const Request &rhs) const;
};

#endif
