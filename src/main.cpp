#include <mpi.h>
#include <set>
#include <iostream>

#include "../inc/HelpRequest.h"
#include "../inc/MPIRequest.h"

using namespace std;

int main(int argc, char** argv) {
    set<HelpRequest> s;
    
    s.insert(HelpRequest(10, 20, 2, 1));
    s.insert(HelpRequest(30, 10, 3, 1));
    s.insert(HelpRequest(5, 30, 1, 1));
    s.insert(HelpRequest(1, 1, 1, 1));
    s.insert(HelpRequest(1, 1, 1, 1));
    s.insert(HelpRequest(1, 1, 1, 1));

    set<HelpRequest>::iterator it;
    
    for (it = s.begin(); it != s.end(); it++) {
        cout << (*it).processId << " " << (*it).timestamp << endl;
    }

    return 0;
}
