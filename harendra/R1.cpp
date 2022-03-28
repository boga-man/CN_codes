#include <iostream>
#include <cstring>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/un.h>
#include <sys/termios.h>
#include <resolv.h>

#define UDS_SERVER_PATH "./uds_socket"

using namespace std;
const int size = 1024;

// connect to uds server
int ConnectToUdsServer(string sun_file)
{
    struct sockaddr_un address;
    int usfd;
    if ((usfd = socket(AF_UNIX, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    cout << "Socket created..." << endl;

    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, sun_file.c_str());

    cout << "Connecting to the server ..." << endl;
    // connect the client socket to server socket
    if (connect(usfd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("connect");
        cout << "Connection failed," << sun_file << endl;
        exit(1);
    }
    printf("Connection established..\n");
    return usfd;
}

int main()
{
    int usfd = ConnectToUdsServer(UDS_SERVER_PATH);
    while(1){
        string report;
        getline(cin, report);
        send(usfd, report.c_str(), report.size(), 0);
    }
    return 0;
}