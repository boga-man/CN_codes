#include <time.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/select.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/un.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/if_ether.h>
#include <net/ethernet.h>
#include <netinet/tcp.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <bits/stdc++.h>
using namespace std;
#define PORT 8080
#define MAXLINE 4096

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

int main(int argc, char *argv[])
{
    int rsfd;
    struct sockaddr_in addr, clientAddr;
    int len = sizeof(clientAddr);
    // get protocol number from the command line
    if (argc != 2)
    {
        cout << "Usage: " << argv[0] << " <protocol number>" << endl;
        return -1;
    }
    int protocol_number = atoi(argv[1]);
    if ((rsfd = socket(AF_INET, SOCK_RAW, protocol_number)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    cout<<"My protocol number is "<<protocol_number<<endl;
    // cout<<"My protocol number is 2"<<endl;
    // if ((rsfd = socket(AF_INET, SOCK_RAW, 2)) < 0)
    // {
    //     perror("socket creation failed");
    //     exit(EXIT_FAILURE);
    // }
    memset(&addr, 0, sizeof(addr));
    memset(&clientAddr, 0, sizeof(clientAddr));

    // optional if only single ip address is present to the system
    // but have to use bind() when multiple ip address is present
    addr.sin_family = AF_INET;
    // addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(rsfd, (const struct sockaddr *)&addr,
         sizeof(addr));

    while (true)
    {
        unsigned char buffer[MAXLINE];
        // this will not only rec message but also the client addr
        int packetSize = recvfrom(rsfd, (char *)buffer, MAXLINE,
                                  0, (sockaddr *)&clientAddr,
                                  (socklen_t *)&len);
        if (packetSize <= 0)
        {
            continue;
        }
        unsigned char tempBuff[MAXLINE];
        for (int i = 0; i < MAXLINE; i++)
            tempBuff[i] = buffer[i];
        struct iphdr *iph = (struct iphdr *)buffer;
        // print_ipheader(iph);

        unsigned short ip_head_len = iph->ihl * 4;
        struct tcphdr *tcp_head = (struct tcphdr *)(tempBuff + ip_head_len);
        cout<<buffer+ip_head_len<<endl;

        memset(buffer, 0, MAXLINE);
    }
    return 0;
}