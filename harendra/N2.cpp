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

#define UDS_SERVER_PATH "./uds_socket"
#define UDS_SERVER_PATH1 "./uds_socket1"

using namespace std;
const int size = 1024;
#define n 3

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

// connect to uds server
int ConnectToUdsServer(string sun_file)
{
    struct sockaddr_un address;
    int usfd;
    if ((usfd = socket(AF_UNIX, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    cout << "Socket created..." << endl;

    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, sun_file.c_str());

    cout << "Connecting to the server ..." << endl;
    // connect the client socket to server socket
    if (connect(usfd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("connect");
        cout << "Connection failed," << sun_file << endl;
        exit(1);
    }
    printf("Connection established..\n");
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

// to connect to a tcp server
int ConnectToTcpServer(string ip, int PORT)
{
    struct sockaddr_in address;
    int csfd;
    if ((csfd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
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
    if (connect(csfd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("connect");
        cout << "Connection failed," << ip << " " << PORT << endl;
    }
    printf("Connection established with ip: %s, port:%d\n", ip.c_str(), PORT);
    return csfd;
}

// initiates TCP server socket
int TcpServerInit(int PORT)
{
    int sfd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    sfd = socket(AF_INET, SOCK_STREAM, 0);
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

int main()
{
    int sfd1 = ConnectToTcpServer("127.0.0.2", 9000);
    char rand;
    read(sfd1, &rand, 1);
    int rsfd = RawSocketInit(2);
    while(1){
        if (rand == '0')
        {
            char buff[size] = {0};
            int k = read(rsfd, buff, size);
            cout << "News from Reporter: " << buff << endl;
            if (strncmp(buff, "8080", 4) == 0)
            {
                int sfd = ConnectToTcpServer("127.0.0.1", 8080);
                cout << "Live Telecast connected\n";
                char msg[size];
                read(sfd, msg, size);
                cout << "News from Live Telecast: " << msg << endl;
                close(sfd);
            }
        }
    }
}