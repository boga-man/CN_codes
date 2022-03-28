#include <iostream>
#include <vector>
#include <cstring>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/un.h>
#include <poll.h>

using namespace std;
const int size = 1024;
#define n 2

// initiates TCP server socket
int TcpServerInit(int PORT)
{
    int sfd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    cout << "Created socket.." << endl;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // binding the address to sfd
    if (bind(sfd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    cout << "Binded socket to address.." << endl;
    return sfd;
}

int main()
{
    int sfd = TcpServerInit(8080);
    listen(sfd, 2);
    while (1)
    {
        int nsfd = accept(sfd, NULL, NULL);
        string s;
        getline(cin, s);
        send(nsfd, s.c_str(), s.size(), 0);
    }
}