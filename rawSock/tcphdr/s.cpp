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
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <semaphore.h>
#include <bits/stdc++.h>
#include <sys/un.h>
using namespace std;

struct ethhdr{
    unsigned char h_dest[6];
    unsigned char h_source[6];
    unsigned short h_proto;
};

void print_ethhdr(struct ethhdr *eth){
    printf("Destination MAC: ");
    for(int i=0;i<6;i++)
        printf("%02x:",eth->h_dest[i]);
    printf("\n");
    printf("Source MAC: ");
    for(int i=0;i<6;i++)
        printf("%02x:",eth->h_source[i]);
    printf("\n");
    /* h_proto gives information about the next layer. If you get 0x800 (ETH_P_IP), it means
that the next header is the IP header */
    printf("Protocol: %04x\n",eth->h_proto);
    cout<<"------------------------------------------------------------\n";
}

void print_iphdr(struct iphdr *ip){
    printf("IP Header\n");
    printf("IP Version: %d\n",ip->version);
    printf("IP Header Length: %d\n",ip->ihl);
    printf("Type of Service: %d\n",ip->tos);
    printf("IP Total Length: %d\n",ntohs(ip->tot_len));
    printf("Identification: %d\n",ntohs(ip->id));
    printf("Time to Live: %d\n",ip->ttl);
    printf("Protocol: %d\n",ip->protocol);
    printf("Checksum: %d\n",ntohs(ip->check));
    cout << "Source ip:" << inet_ntoa(*(in_addr *)&ip->saddr) << endl;
    cout << "Destination ip:" << inet_ntoa(*(in_addr *)&ip->daddr) << endl;
    cout<<"------------------------------------------------------------\n";
}

void print_tcphdr(struct tcphdr *tcp){
    printf("TCP Header\n");
    printf("Source Port: %d\n",ntohs(tcp->source));
    printf("Destination Port: %d\n",ntohs(tcp->dest));
    printf("Sequence Number: %d\n",ntohl(tcp->seq));
    printf("Acknowledgement Number: %d\n",ntohl(tcp->ack_seq));
    printf("Header Length: %d\n",tcp->doff);
    printf("Urgent Flag: %d\n",tcp->urg);
    printf("Acknowledgement Flag: %d\n",tcp->ack);
    printf("Push Flag: %d\n",tcp->psh);
    printf("Reset Flag: %d\n",tcp->rst);
    printf("Synchronize Flag: %d\n",tcp->syn);
    printf("Finish Flag: %d\n",tcp->fin);
    printf("Window: %d\n",ntohs(tcp->window));
    printf("Checksum: %d\n",ntohs(tcp->check));
    printf("Urgent Pointer: %d\n",tcp->urg_ptr);
    cout<<"------------------------------------------------------------\n";
}

int main(){
    int rsfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if(rsfd<0){
        perror("socket");
        exit(1);
    }

    while(1){

        // receiving the data on the socket
        char buff[2048];
        int n = recvfrom(rsfd, buff, 2048, 0, NULL, NULL);
        if(n<0){
            perror("recvfrom");
            exit(1);
        }

        // extract ether header
        // struct ethhdr *eth = (struct ethhdr *)buff;
        // print_ethhdr(eth);

        // extract ip header
        struct iphdr *ip = (struct iphdr *)(buff);
        print_iphdr(ip);

        // extract tcp header
        /* getting actual size of IP header*/
        int iphdrlen = ip->ihl*4;
        /* getting pointer to udp header*/
        struct tcphdr *tcp = (struct tcphdr*)(buff + iphdrlen);
        print_tcphdr(tcp);

        // extract the payload
        cout<<"Payload: "<<buff+iphdrlen<<endl;
        cout<<"\n\n\n\n";
        memset(buff, 0, sizeof(buff));
    }
    return 0;

}