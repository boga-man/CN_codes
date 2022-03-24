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
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <semaphore.h>
#include <bits/stdc++.h>
using namespace std;

int main()
{
    int in = open("temp.cpp", O_RDONLY);

    int sfd, nsfd, bytesread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int PORT = 8080;
    int opt = 1;

    // create the socket endpoint
    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    cout << "Socket created\n";
    // setup the socket options
    if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsocketopt failed");
        exit(EXIT_FAILURE);
    }

    // setup the address struct
    address.sin_family = AF_INET;
    // setup the port number
    address.sin_port = htons(PORT);
    // setup the ip address
    if (inet_pton(AF_INET, "127.0.0.1", &address.sin_addr) <= 0)
    {
        perror("inet_pton failed");
        exit(EXIT_FAILURE);
    }

    // connect to the server
    if (connect(sfd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("connect failed");
        exit(EXIT_FAILURE);
    }
    cout << "Connected to the server\n";

    // sending temp.cpp to the server
    char code[16];
    string s = "false";
    while ((bytesread = read(in, code, 16)) > 0)
    {
        send(sfd, code, bytesread, 0);
    }
    cout << "Code sent to the server\n";
    // receiving the verdict from the server
    char buffer[1024] = {0};

    // reading the verdict from the server
    bytesread = read(sfd, buffer, 1024);
    string verdict(buffer);
    cout << "Verdict by the server: " << verdict << endl;

    return 0;
}