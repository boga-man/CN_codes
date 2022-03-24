
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
#include <arpa/inet.h>
#include <poll.h>
using namespace std;

// store all the clients connected to the server
vector<int> nsfds;
// store the tids of all the threads created
vector<int> tids;
// store the server socket fd
int sfd;

// to hold a server's information
struct ServerData
{
    string ip;
    int port;
};

// to read data from a file 
string ReadFD(int fd)
{
    string s = "";
    char ch;
    while (true)
    {
        int n = read(fd, &ch, 1);
        if (n <= 0 || ch == '\0')
        {
            break;
        }
        s.push_back(ch);
    }
    return s;
}

// to write data to a file
int WriteFD(int fd, string s)
{
    s.push_back('\0');
    return send(fd, s.c_str(), s.size(), 0);
}

void CloseHandler(int count)
{
    cout << "Closing the server...\n";
    for (int i = 0; i < nsfds.size(); i++)
        close(nsfds[i]);
    for (int i = 0; i < tids.size(); i++)
        pthread_cancel(tids[i]);
    close(sfd);
    exit(0);
}

// function to be used with a new thread that serves a client
void *ClientHandler(void *arg)
{
    int nsfd = *(int *)arg;
    int valread;
    char buffer[1024] = {0};
    while (1)
    {
        valread = read(nsfd, buffer, 1024);
        string su(buffer);
        if (valread == 0)
        {
            cout << "Client disconnected" << endl;
            break;
            close(nsfd);
        }

        cout << "Client sent: " << buffer << endl;
        memset(buffer, 0, 1024);
        transform(su.begin(), su.end(), su.begin(), ::toupper);
        su = "From capitalizer server: " + su;
        strcpy(buffer, su.c_str());
        send(nsfd, buffer, su.size(), 0);
        memset(buffer, 0, 1024);
    }
    return NULL;
}

// initiates TCP server socket
int TcpServerInit(int PORT)
{
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
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

// to connect to a tcp server
int ConnectToTcpServer(string ip, int PORT)
{
    struct sockaddr_in address;
    int sfd;
    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    cout << "Socket created..." << endl;

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    if (inet_pton(AF_INET, ip.c_str(), &address.sin_addr) <= 0)
    {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    cout << "Connecting to the server ..." << endl;
    // connect the client socket to server socket
    if (connect(sfd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("connect");
        cout << "Connection failed," << ip << " " << PORT << endl;
    }
    printf("Connection established with ip: %s, port:%d\n", ip.c_str(), PORT);
    return sfd;
}