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

int cnt = 0;

void handler(int sig){
    cout<<"Packets captured so far: "<<cnt<<endl;
    kill(getpid(),SIGTERM);
}
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
int main()
{
    // creating signal handler
    signal(SIGINT, handler);

    // creating a raw socket to sniff
    int sfd;
    struct sockaddr_in clientAddr;
    int len = sizeof(clientAddr);
    if ((sfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    memset(&clientAddr, 0, sizeof(clientAddr));

    while (true)
    {
        unsigned char buffer[MAXLINE];
        // this will not only rec message but also the client addr
        int packetSize = recvfrom(sfd, (char *)buffer, MAXLINE,
                                  0, (sockaddr *)&clientAddr,
                                  (socklen_t *)&len);
        if (packetSize <= 0)
        {
            continue;
        }
        
        // extract and print the ip header
        struct iphdr *iph = (struct iphdr *)buffer;
        print_ipheader(iph);

        memset(buffer, 0, MAXLINE);
        cnt++;
    }
    return 0;
}