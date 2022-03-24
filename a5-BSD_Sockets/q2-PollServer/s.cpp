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
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <netinet/in.h>
#include <poll.h>
#include <pthread.h>
using namespace std;
// function to handle recive data from client and print it
void reciveData(int nsfd)
{
    char buffer[1024] = {0};
    int valread;
    // while (true)
    {
        valread = read(nsfd, buffer, 1024);
        if (valread == 0)
        {
            cout << "Client disconnected" << endl;
            return;
        }
        cout << buffer << endl;
    }
}
// funtion to handle recive data from client and capitalize it and send it back
void capitalizeData(int nsfd)
{
    char buffer[1024] = {0};
    int valread;
    // while (true)
    {
        valread = read(nsfd, buffer, 1024);
        if (valread == 0)
        {
            cout << "Client disconnected" << endl;
            return;
        }
        for (int i = 0; i < strlen(buffer); i++)
        {
            buffer[i] = toupper(buffer[i]);
        }
        write(nsfd, buffer, strlen(buffer));
    }
}

// function to handle recive data from client and make it lowercase and send it back
void lowercaseData(int nsfd)
{
    char buffer[1024] = {0};
    int valread;

    valread = read(nsfd, buffer, 1024);
    if (valread == 0)
    {
        cout << "Client disconnected" << endl;
        return;
    }
    for (int i = 0; i < strlen(buffer); i++)
    {
        buffer[i] = tolower(buffer[i]);
    }
    write(nsfd, buffer, strlen(buffer));
}

int main()
{
    int sfd[3], nsfd[3], valread[3];
    struct sockaddr_in address[3];
    char buffer[1024] = {0};
    int PORT[3];
    for (int i = 0; i < 3; i++)
    {
        PORT[i] = 8080 + i;
    }
    int addrlen = sizeof(address[0]);
    for (int i = 0; i < 3; i++)
    {
        if ((sfd[i] = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        {
            perror("failed creating socket");
            exit(EXIT_FAILURE);
        }
        cout << "Socket created" << endl;
        address[i].sin_family = AF_INET;
        address[i].sin_addr.s_addr = INADDR_ANY;
        address[i].sin_port = htons(PORT[i]);

        // set the socket options
        int opt = 1;
        if (setsockopt(sfd[i], SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
        {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }

        if (bind(sfd[i], (struct sockaddr *)&address[i], sizeof(address[i])) < 0)
        {
            perror("failed binding");
            exit(EXIT_FAILURE);
        }
        cout << "Socket binded" << endl;
        struct sockaddr_in serverAddress;
        
        if (listen(sfd[i], SOMAXCONN) < 0)
        {
            perror("failed listening");
            exit(EXIT_FAILURE);
        }
    }

    // poll all sfd
    struct pollfd fds[3];
    for (int i = 0; i < 3; i++)
    {
        fds[i].fd = sfd[i];
        fds[i].events = POLLIN;
    }
    while (true)
    {
        int ret = poll(fds, 3, 200);
        if (ret < 0)
        {
            perror("poll failed");
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < 3; i++)
        {
            if (fds[i].revents & POLLIN)
            {
                if ((nsfd[i] = accept(sfd[i], (struct sockaddr *)&address[i], (socklen_t *)&addrlen)) < 0)
                {
                    perror("failed accepting");
                    exit(EXIT_FAILURE);
                }
                cout << "Client connected" << endl;
                // create pthread to handle recive data from client and print it
                pthread_t thread;
                if (i == 0)
                {
                    pthread_create(&thread, NULL, (void *(*)(void *))reciveData, (void *)nsfd[i]);
                }
                else if (i == 1)
                {
                    pthread_create(&thread, NULL, (void *(*)(void *))capitalizeData, (void *)nsfd[i]);
                }
                else if (i == 2)
                {
                    pthread_create(&thread, NULL, (void *(*)(void *))lowercaseData, (void *)nsfd[i]);
                }
            }
        }
    }
    close(sfd[0]);
    close(sfd[1]);
    close(sfd[2]);
    close(nsfd[0]);
    close(nsfd[1]);
    close(nsfd[2]);
    return 0;
}