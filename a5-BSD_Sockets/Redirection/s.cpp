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
using namespace std;

int main()
{
    int PORT = 8080;
    int sfd, nsfd, valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // Creating socket file descriptor
    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    // bind the address to the socket
    if (bind(sfd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // listen to the socket
    if (listen(sfd, SOMAXCONN) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // accept and incoming connection
    while(1){
        if ((nsfd = accept(sfd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        // create a child process to handle the client
        pid_t pid = fork();
        if (pid == 0)
        {
            int temp = nsfd;
            // child process
            while (1)
            {
                valread = read(temp, buffer, 1024);
                string su(buffer);
                if (valread == 0)
                {
                    cout << "Client disconnected" << endl;
                    break;
                }
                cout << buffer << endl;
                memset(buffer, 0, 1024);
                transform(su.begin(), su.end(), su.begin(), ::toupper);
                su = "From capitalizer server: "+ su;
                strcpy(buffer, su.c_str());
                send(temp, buffer, su.size(), 0);
                memset(buffer, 0, 1024);
            }
            break;
        }
        else
        {
            // parent process
            close(nsfd);
        }
    }
    wait(NULL);

    // close the sockets
    
    close(sfd);
    return 0;
}