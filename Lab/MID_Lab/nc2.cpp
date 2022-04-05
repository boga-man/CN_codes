#include "myHeader.h"

int main(){
    // first connect with nc via uds
    int usfd = ConnectToUdsServer(usfd_path);

    // --------------------- usfd working ------------------

    // TODO: NC2 should be waiting for sfd from NC1
    // connecting to the services available in a separate thread
}