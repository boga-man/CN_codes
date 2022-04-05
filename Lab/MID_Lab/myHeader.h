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
#include <netinet/tcp.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <bits/stdc++.h>
#include <arpa/inet.h>
#include <poll.h>
using namespace std;

/* 
    NC1 TCP port for packages - 3000
    NC2 TCP port for packages - 3001
    NC1 TCP port for clients - 3002
    NC2 doesn't connect with clients directly
    p1 - sms packages
    p2 - live packages
*/

int NC1_PACK_PORT = 3001;
int NC2_PACK_PORT = 3002;
int NC1_CLI_PORT = 3003;
int NC1_UDP_PORT = 3004;

vector<int> p_sms_udp_ports;
vector<int> p_live_udp_ports;

string sysIp = "127.0.0.1";

// to hold a server's information
struct ServerData
{
    string ip;
    int port;
};

// store all the clients connected to the server
vector<int> nsfds;
// store the tids of all the threads created
vector<int> tids;

// ==================================== file operations ====================================
// to read data from a file
string ReadFD(int fd)
{
    string s = "";
    char ch;
    while (true)
    {
        int n = recv(fd, &ch, 1, 0);
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


// ===================================== UDS Sockets ==================================================
// uds server path
string usfd_path = "./usfd";

// uds client path
string client_socket_path = "./client_socket";

// creating uds server socket
int createListenUsfd(string socket_path)
{
    // creating socket
    int usfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (usfd == -1)
    {
        perror("socket");
        exit(1);
    }

    // creating server socket address
    struct sockaddr_un saddr;
    saddr.sun_family = AF_UNIX;
    strcpy(saddr.sun_path, socket_path.c_str());

    // binding socket to server socket address
    if (bind(usfd, (struct sockaddr *)&saddr, sizeof(saddr)) == -1)
    {
        perror("bind");
        exit(1);
    }

    // listening for connections
    if (listen(usfd, 5) == -1)
    {
        perror("listen");
        exit(1);
    }
    cout<<"Listening for connections on UDS\n";

    return usfd;
}

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

// =========================================== fd passing codes =========================================
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

// ====================================== Raw Sockets =====================================================
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

// print the ip header of the packet
void print_ipheader(struct iphdr *ip)
{
    cout << "------------------------\n";
    cout << "Printing IP header....\n";
    cout << "IP version:" << (unsigned int)ip->version << endl;
    cout << "IP header length:" << (unsigned int)ip->ihl << endl;
    cout << "Type of service:" << (unsigned int)ip->tos << endl;
    cout << "Total ip packet length:" << ntohs(ip->tot_len) << endl;
    cout << "Packet id:" << ntohs(ip->id) << endl;
    cout << "Time to leave :" << (unsigned int)ip->ttl << endl;
    cout << "Protocol:" << (unsigned int)ip->protocol << endl;
    cout << "Check:" << ip->check << endl;
    cout << "Source ip:" << inet_ntoa(*(in_addr *)&ip->saddr) << endl;
    cout << "Destination ip:" << inet_ntoa(*(in_addr *)&ip->daddr) << endl;
    cout << "Payload: ";
    cout << "End of IP header\n";
    cout << "------------------------\n";
}

// print the payload of the packet
void print_payload(char *buff)
{
    struct iphdr *iph = (struct iphdr *)buff;
    cout<<buff+iph->ihl*4<<endl;
}

// ===================================== Thread functions ================================================
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

// =================================== TcpServerInit =====================================================
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

    int opt = 1;
    if(setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

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
    struct sockaddr_in address, caddr;
    int sfd;
    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    cout << "Socket created..." << endl;

    // caddr.sin_family = AF_INET;
    // caddr.sin_addr.s_addr = INADDR_ANY;
    // caddr.sin_port = htons(CPORT);

    // // binding the address to sfd
    // if (bind(sfd, (struct sockaddr *)&caddr, sizeof(caddr)) < 0)
    // {
    //     perror("bind failed");
    //     exit(EXIT_FAILURE);
    // }

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
        return -1;
    }
    printf("Connection established with ip: %s, port:%d\n", ip.c_str(), PORT);
    return sfd;
}

void printIPandPort(struct sockaddr_in address, string whose) {
    char myIP[16];
    inet_ntop(AF_INET, &address.sin_addr, myIP, sizeof(myIP));
    int myPort = ntohs(address.sin_port);
    cout << whose << " IP: " << myIP << endl;
    cout << whose << " Port: " << myPort << endl;
}


void getIpAndPort(string &ip, string &port, string packAddr){
    int i = 0;
    while (packAddr[i] != ';')
    {
        ip += packAddr[i];
        i++;
    }
    i++;
    while (packAddr[i] != '\0')
    {
        port += packAddr[i];
        i++;
    }
}

// ======================================= MQ ================================================
struct msg_st
{
    long int type;
    char msg_text[128];
};

int initializeMQ(string mqName) {
    int key = ftok(mqName.c_str(), 65);
    int mqId = msgget(key, 0666 | IPC_CREAT);
    return mqId;
}

void sendInMQ(int mqId, long int msgType, string message) {
    message.push_back('\0');
    msg_st msg;
    msg.type = msgType;
    strcpy(msg.msg_text, message.c_str());
    msgsnd(mqId, &msg, sizeof(msg.msg_text), 0);
}

string recFromMQ(int mqId, long int msgType = 0) {
    msg_st msg;

    msgrcv(mqId, &msg, sizeof(msg.msg_text), msgType, 0);

    return string(msg.msg_text);
}




