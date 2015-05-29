#ifndef HELP_REQUEST_H
#define HELP_REQUEST_H

struct HelpRequest {
    int processId;
    int timestamp;
    int currentTime;
    int weight;

    HelpRequest();
    HelpRequest(int processId, int timestamp, int currentTime, int weight);
    bool operator<(const HelpRequest &rhs) const;
};

#endif
