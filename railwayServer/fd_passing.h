#include <stdio.h>
#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <bits/stdc++.h>
#include <arpa/inet.h>
#include <poll.h>
using namespace std;

string server_socket_path = "./server_socket";
int adProtoNum = 2;

// handler to display ads in platform
void *display_ads(void *arg)
{
    // get the rsfd from arg
    int rsfd = *((int *)arg);

    // keep reading from rsfd and display the message payload
    while (1)
    {
        // read from rsfd
        char buf[1024];
        int n = read(rsfd, buf, 1024);
        if (n == 0)
        {
            cout << "Server closed the connection" << endl;
            break;
        }
        else if (n < 0)
        {
            cout << "Error reading from rsfd" << endl;
            break;
        }
        else
        {
            struct iphdr *ip;
            ip = (struct iphdr *)buf;
            // display the message payload
            cout << (buf + (ip->ihl) * 4) << endl;
        }
    }

    return NULL;
}


// fd passing codes
void send_fd(int socket, int fds) // send fd by socket
{
    struct msghdr msg = {0};
    struct cmsghdr *cmsg;
    char buf[CMSG_SPACE(sizeof(int))], dup[256];
    memset(buf, '\0', sizeof(buf));
    struct iovec io = {.iov_base = &dup, .iov_len = sizeof(dup)};

    msg.msg_iov = &io;
    msg.msg_iovlen = 1;
    msg.msg_control = buf;
    msg.msg_controllen = sizeof(buf);

    cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(sizeof(int));

    memcpy((int *)CMSG_DATA(cmsg), &fds, sizeof(int));

    if (sendmsg(socket, &msg, 0) < 0)
        perror("Failed to send message");
}
int recv_fd(int socket)
{

    int *fds = new int;
    struct msghdr msg = {0};
    struct cmsghdr *cmsg;
    char buf[CMSG_SPACE(sizeof(int))], dup[256];
    memset(buf, '\0', sizeof(buf));
    struct iovec io = {.iov_base = &dup, .iov_len = sizeof(dup)};

    msg.msg_iov = &io;
    msg.msg_iovlen = 1;
    msg.msg_control = buf;
    msg.msg_controllen = sizeof(buf);

    if (recvmsg(socket, &msg, 0) < 0)
        perror("Failed to receive message");

    cmsg = CMSG_FIRSTHDR(&msg);

    memcpy(fds, (int *)CMSG_DATA(cmsg), sizeof(int));

    return *fds;
}

// create a raw socket
int RawSocketInit(int protoNum)
{
    int rsfd;
    if ((rsfd = socket(AF_INET, SOCK_RAW, protoNum)) == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    return rsfd;
}