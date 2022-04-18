#include <stdio.h>
#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include <bits/stdc++.h>
using namespace std;

struct ServerData{
    string ip;
    int port;
};

struct ServerDataString{
    string serverData;
};

// static values
const string AS_ADDR = "#127.0.0.1@4000";
const string S_ADDR = "#127.0.0.1@3000"

// get the foreign ipAddress and foreign port of a connection
ServerData getSockName(int sfd)
{
    struct sockaddr_in address;
    int addLen = sizeof(address);
    if (getsockname(sfd, (struct sockaddr *)&address, &addLen) == -1)
    {
        perror("getsockname() failed\n");
        exit(0);
    }

    ServerData data;
    string ip(inet_ntoa(address.sin_addr));
    data.ip = ip;
    data.port = (int)ntohs(address.sin_port);

    return data;
}

ServerData getPeerName(int nsfd)
{
    struct sockaddr_in address;
    int addLen = sizeof(address);
    if (getpeername(nsfd, (struct sockaddr *)&address, &addLen) == -1)
    {
        perror("getpeername() failed\n");
        exit(0);
    }

    ServerData data;
    string ip(inet_ntoa(address.sin_addr));
    data.ip = ip;
    data.port = (int)ntohs(address.sin_port);

    return data;
}

// starts a tcp server at provided port in this system
int InitTcpServer(int PORT){
    int sfd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    cout << "Created server socket..." << endl;

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // binding the address to sfd
    if (bind(sfd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    cout << "Binded socket to address..." << endl;
    return sfd;
}

// repeatedly tries to connect to a server till connection is established or tries are exhausted
void repeatedTryConnect(int sfd, int tries, struct sockaddr_in &address){
    while(tries){
        if (connect(sfd, (struct sockaddr *)&address, sizeof(address)) < 0)
        {
            perror("connect");
            cout << "Connection failed,"<< endl;
        }
        cout<<"Retrying after 5 seconds\n";
        sleep(5);
        tries--;
    }
}

// connects to an external (or internal) tcp server
int ConnectToTcpServer(string ip, int PORT){
    struct sockaddr_in address;
    int sfd;
    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    cout << "Client Socket created..." << endl;

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    if (inet_pton(AF_INET, ip.c_str(), &address.sin_addr) <= 0)
    {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    cout << "Connecting to the server ..." << endl;
    // connect the client socket to server socket
    repeatedTryConnect(sfd, 10, address);
    printf("Connection established with ip: %s, port:%d\n", ip.c_str(), PORT);
    return sfd;
}

void *ClientSerivceHandler(void *arg)
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
        // capitaliser service
        transform(su.begin(), su.end(), su.begin(), ::toupper);
        su = "From capitalizer server: " + su;
        strcpy(buffer, su.c_str());
        send(nsfd, buffer, su.size(), 0);
        memset(buffer, 0, 1024);
    }
    return NULL;
}

void *MaintServiceHandler(void *arg){
    int nsfd = *(int *)arg;
    char buffer[1024] = {0};
    int valread;
    valread = read(nsfd, buffer, 1024);
    string su(buffer);
    if (valread == 0)
    {
        cout << "Client disconnected" << endl;
        close(nsfd);
    }

    // send the message to the client containing the AS server address
    send(nsfd, AS_ADDR.c_str(), AS_ADDR.size(), 0);

    return NULL;
}

// Read and write from a file desciptor
string Readfd(int fd)
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

int Writefd(int fd, string s)
{
    s.push_back('\0');
    return send(fd, s.c_str(), s.size(), 0);
}

// a function to convert ServerData into a DataString
ServerDataString getServerDataString(ServerData data){
    string ipPort = '#' + data.ip + "@" + to_string(data.port) + "\0";
    ServerDataString dataString;
    dataString.serverData = ipPort;
    
    return dataString;
}

ServerData IntepretServerChangeMessage(string s)
{
    ServerData data;
    data.ip = "";
    data.port = -1;
    bool delim = false;
    string ipS = "";
    for (int i = 1; i < s.size(); i++)
    {
        if (s[i] == '@')
        {
            delim = true;
            continue;
        }
        if (!delim)
        {
            data.ip.push_back(s[i]);
        }
        else
        {
            ipS.push_back(s[i]);
        }
    }
    data.port = atoi(ipS.c_str());
    return data;
}
