#ifndef REQUEST_H
#define REQUEST_H

struct Request {
    int processId;
    int timestamp;
    int currentTime;

    Request(int processId, int bimestamp);
    bool operator<(const Request &rhs) const;
};

#endif
