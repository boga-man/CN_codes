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
#include <poll.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <semaphore.h>
#include <bits/stdc++.h>
#include <sys/un.h>
#include "fd_passing.h"
using namespace std;

int get_avail_pform(int avail[], int n)
{
    int ret = 0;
    // identify first index with avail[i] = 1;
    for (int i = 0; i < n; i++)
    {
        if (avail[i] == 1)
        {
            return i;
        }
    }

    return -1;
}

void bcast_msg_to_pforms(int psfds[], int no_pforms)
{
    // broadcast the message to all the connected platforms
    for (int i = 0; i < no_pforms; i++)
    {
        string message = "A train has arrived at the NITW station\n";
        if (send(psfds[i], message.c_str(), sizeof(message.c_str()), 0) == -1)
        {
            perror("read");
            exit(1);
        }
    }
}

int main()
{
    int no_pforms = 3;
    int tsfd[3] = {0}, psfd, psfds[no_pforms];
    int available_platform[3] = {0};
    struct sockaddr_in taddr[3];
    struct sockaddr_un paddr_un;
    char buf[1024];
    int opt = 1;
    int PORT = 5000;
    string server_socket_path = "./server_socket";

    // creating rset for reading fds from server
    // fd_set rset;

    // creating unix socket for communication with server
    memset(&paddr_un, 0, sizeof(paddr_un));

    // creating the unix socket
    // platforms first for connections via psfd
    if ((psfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    cout << "Waiting for platforms to connect\n";

    // setting up the socket address
    paddr_un.sun_family = AF_UNIX;
    strcpy(paddr_un.sun_path, server_socket_path.c_str());
    int len = sizeof(paddr_un);

    // unlink socket path so that it can be bind will succeed
    unlink(paddr_un.sun_path);

    // bind the socket to the address
    if (bind(psfd, (struct sockaddr *)&paddr_un, len) == -1)
    {
        perror("bind");
        exit(1);
    }

    // listening for connections
    if (listen(psfd, 3) == -1)
    {
        perror("listen");
        exit(1);
    }

    cout<<"Waiting for platforms\n";
    // accept the connections with platforms when the server is ready to listen
    for (int i = 0; i < 3; i++)
    {
        if ((psfds[i] = accept(psfd, (struct sockaddr *)&paddr_un, (socklen_t *)&len)) == -1)
        {
            perror("accept");
            exit(1);
        }
        cout<<"Accepted a platform\n";
        available_platform[i] = 1;
    }

    cout<<"Opening sockets for trains\n";
    // opening sockets for three different points
    for (int i = 0; i < no_pforms; i++)
    {
        if ((tsfd[i] = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        {
            perror("socket");
            exit(1);
        }

        if (setsockopt(tsfd[i], SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1)
        {
            perror("setsockopt");
            exit(1);
        }

        taddr[i].sin_family = AF_INET;
        taddr[i].sin_addr.s_addr = INADDR_ANY;
        taddr[i].sin_port = htons(PORT + i + 1);

        if (bind(tsfd[i], (struct sockaddr *)&taddr[i], sizeof(taddr[i])) == -1)
        {
            perror("bind");
            exit(1);
        }

        if (listen(tsfd[i], 3) == -1)
        {
            perror("listen");
            exit(1);
        }
    }
    
    cout<<"Starting polling on train fds\n";
    struct pollfd pfds[no_pforms];
    for(int i = 0; i<no_pforms; i++){
        pfds[i].fd = tsfd[i];
        pfds[i].events = POLL_IN;
        pfds[i].revents = 0;
    }
    
    len = sizeof(taddr[0]);
    // select fds
    while (1)
    {
        int ready = poll(pfds, no_pforms, 5000);

        // if tsfd[i] is ready, accept the connections only if the platform is available
        int nsfd;
        int idx;
        if(ready>0 && (idx = get_avail_pform(available_platform, no_pforms)) != -1){
            for (int i = 0; i < 3 && ready > 0; i++)
            {
                if (((idx = get_avail_pform(available_platform, no_pforms) != -1) && (POLL_IN & pfds[i].revents)))
                {
                    available_platform[idx] = 0;
                    if (nsfd = (accept(tsfd[i], (struct sockaddr *)&taddr[0], (socklen_t *)&len)) == -1)
                    {
                        perror("accept");
                        exit(1);
                    }
                    cout<<"New train entering the station\n";

                    // notifying all the platforms
                    // bcast_msg_to_pforms(psfds, no_pforms);

                    send_fd(psfds[i], nsfd);
                    close(nsfd);
                    recv_fd(psfds[i]);

                    cout<<"Train has left the station\n";
                    available_platform[idx] = 1;
                }
            }
        }
    }
}