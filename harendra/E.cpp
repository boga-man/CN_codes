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
#include <pthread.h>

#define UDS_SERVER_PATH "./uds_socket"
#define UDS_SERVER_PATH1 "./uds_socket1"

int usfd_d, rsfd;

using namespace std;
const int size = 1024;
#define n 3
int m = 4;
// initiate UDS server socket
int UdsServerInit(string sun_file)
{
    int usfd;
    unlink(sun_file.c_str());
    struct sockaddr_un server;
    usfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (usfd == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, sun_file.c_str());
    unlink(server.sun_path);
    int len = strlen(server.sun_path) + sizeof(server.sun_family);
    if (bind(usfd, (struct sockaddr *)&server, len) == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    cout << "Initiated UDS server" << endl;
    return usfd;
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

void* reporterHandler(void *arg){
    int usfd = *(int *)arg;
    while(1){
        cout<<"Waiting for news\n";
        char buff[size] = {0};
        int k = read(usfd, buff, size);
        if (k > 0)
        {
            if (strncmp(buff, "/d", 2) == 0)
            {
                // if report is starting from "/d" then we have to pass fd of that reporter to Document writer
                // so creating uds connection with Document writer
                send_fd(usfd_d, usfd);
                cout << "passed fd to Document Writer\n";
            }
            else
            {
                // if report is not string from "/d" then we have to send this report to news reader
                // so for that we will create raw socket
                struct sockaddr_in client;
                client.sin_family=AF_INET;
                client.sin_addr.s_addr=inet_addr("127.0.0.1");
                sendto(rsfd, buff, size, 0, (struct sockaddr*)&client, sizeof(client));
            }
        }else{
            cout<<"No news\n";
            return NULL;
        }
    }
}

int main()
{

    int usfd[n];
    // initiate uds server
    int Usfd = UdsServerInit(UDS_SERVER_PATH);
    listen(Usfd, n);

    // For Document Writer
    usfd_d = UdsServerInit(UDS_SERVER_PATH1);
    listen(usfd_d, n);

    rsfd = RawSocketInit(2);
    cout << "Raw socket created\n";

    // connecting the news reporters
    for (int i = 0; i < n; i++)
    {
        usfd[i] = accept(Usfd, NULL, NULL);
        pthread_t tid;
        pthread_create(&tid, NULL, reporterHandler, (void *)&usfd[i]);
    }
    cout << "connected to Reporters\n";

    int nusfd_d = accept(usfd_d, NULL, NULL);
    cout << "connected to Document Writer\n";

    while(1);

    // TODO: remove the poll, put a thread for each news reporter
    // getting the news from repoters
    // use poll to know which news reporter has sent data
    // struct pollfd pfds[n];
    // for (int i = 0; i < n; i++)
    // {
    //     pfds[i].events = POLLIN;
    //     pfds[i].revents = 0;
    // }
    // while(1){
    //     int ready = poll(pfds, n, -1);
    //     cout<<ready<<endl;
    //     for (int i = 0; i < n; i++)
    //     {
    //         if(pfds[i].revents & POLLIN){
    //             char buff[size] = {0};
    //             int k = read(pfds[i].fd, buff, size);
    //             if (k > 0)
    //             {
    //                 if (strncmp(buff, "/d", 2) == 0)
    //                 {
    //                     // if report is starting from "/d" then we have to pass fd of that reporter to Document writer
    //                     // so creating uds connection with Document writer
    //                     send_fd(usfd_d, pfds[i].fd);
    //                     cout << "passed fd to Document Writer\n";
    //                 }
    //                 else
    //                 {
    //                     // if report is not string from "/d" then we have to send this report to news reader
    //                     // so for that we will create raw socket
    //                     send(rsfd, buff, k, 0);
    //                 }
    //             }
    //         }
    //     }
    // }
}