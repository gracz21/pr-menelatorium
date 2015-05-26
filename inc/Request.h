#ifndef REQUEST_H
#define REQUEST_H

struct Request {
    int processId;
    int timestamp;
    int currentTime;

    Request();
    Request(int processId, int timestamp, int currentTime);
    bool operator<(const Request &rhs) const;
};

#endif
