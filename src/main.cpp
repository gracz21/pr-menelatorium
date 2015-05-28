#include <mpi.h>
#include <set>
#include <iostream>

#include "../inc/HelpRequest.h"
#include "../inc/MPIRequest.h"

using namespace std;

int main(int argc, char** argv) {
    set<HelpRequest> s;
    
    s.insert(HelpRequest(10, 20, 2, 2));
    s.insert(HelpRequest(10, 20, 3, 1));
    s.insert(HelpRequest(10, 30, 3, 1));
    s.insert(HelpRequest(20, 20, 3, 1));
    s.insert(HelpRequest(10, 20, 3, 1));

    set<HelpRequest>::iterator it;
    for (it = s.begin(); it != s.end(); it++) {
        cout << (*it).processId << endl;
    }

    return 0;
}
