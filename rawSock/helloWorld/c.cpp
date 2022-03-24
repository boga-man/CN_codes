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
#include <arpa/inet.h>
#include <bits/stdc++.h>
using namespace std;
#define PORT 8080
#define MAXLINE 1024
#define MSG_CONFIRM 0x800
int main()
{
    // creating a socket
    int sfd;
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    if ((sfd = socket(AF_INET, SOCK_RAW, 2)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // send a message to the server
    string s = "";
    char buffer[MAXLINE];
    cout << "Enter text : ";
    getline(cin, s);
    sendto(sfd, s.c_str(), s.size(),
            0, (const struct sockaddr *)&addr, sizeof(addr));
    memset(buffer, 0, MAXLINE);

    close(sfd);
    return 0;
}