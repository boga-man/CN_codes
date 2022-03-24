#include <stdio.h>
#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <bits/stdc++.h>
#include <poll.h>
using namespace std;
void service3(int nsfd)
{
    int valread;
    char buffer[1024] = {0};
    while (1)
    {
        valread = read(nsfd, buffer, 1024);
        string s(buffer);
        if (valread == 0)
        {
            cout << "Client disconnected" << endl;
            break;
            close(nsfd);
        }

        cout << "Client sent: " << buffer << endl;
        memset(buffer, 0, 1024);
        s = "From service 3: received\n";
        strcpy(buffer, s.c_str());
        send(nsfd, buffer, s.size(), 0);
        memset(buffer, 0, 1024);
    }
}

void service1(int nsfd)
{
    int valread;
    char buffer[1024] = {0};
    while (1)
    {
        valread = read(nsfd, buffer, 1024);
        string s(buffer);
        if (valread == 0)
        {
            cout << "Client disconnected" << endl;
            break;
            close(nsfd);
        }

        memset(buffer, 0, 1024);
        // changing the string to upper case
        transform(s.begin(), s.end(), s.begin(), ::toupper);
        s = "From service 1: " + s;
        strcpy(buffer, s.c_str());
        send(nsfd, buffer, s.size(), 0);
        memset(buffer, 0, 1024);
    }
}

void service2(int nsfd)
{
    int valread;
    char buffer[1024] = {0};
    while (1)
    {
        valread = read(nsfd, buffer, 1024);
        string s(buffer);
        if (valread == 0)
        {
            cout << "Client disconnected" << endl;
            break;
            close(nsfd);
        }

        memset(buffer, 0, 1024);
        // changing the string to lower case
        transform(s.begin(), s.end(), s.begin(), ::tolower);
        s = "From service 2: " + s;
        strcpy(buffer, s.c_str());
        send(nsfd, buffer, s.size(), 0);
        memset(buffer, 0, 1024);
    }
}
void handleRequest(int nsfd)
{
    int valread;
    char c;
    valread = read(nsfd, &c, 1);
    if (valread == 0)
    {
        cout << "Client disconnected" << endl;
        return;
    }
    int srvc = c;
    cout << "Service requested is: " << srvc << endl;
    switch(srvc){
        case 1:
            service1(nsfd);
            break;
        case 2:
            service2(nsfd);
            break;
        case 3:
            service3(nsfd);
            break;
        default:
            cout << "Invalid service requested" << endl;
            break;
    }
    cout<<"-----------------------------------------------------\n";
}
int main()
{
    int PORT = 8000;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    // creating the socket
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    // binding the socket
    bind(sfd, (struct sockaddr *)&address, sizeof(address));

    // listening for requests
    listen(sfd, 3);
    for (;;)
    {
        struct sockaddr_in cliaddr;
        int addrlen = sizeof(cliaddr);
        cout << "Waiting for connections..." << endl;
        int nsfd = accept(sfd, (struct sockaddr *)&cliaddr, (socklen_t *)&addrlen);
        if (nsfd < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        cout << "New client connected..." << endl;
        handleRequest(nsfd);
    }
    return 0;
}