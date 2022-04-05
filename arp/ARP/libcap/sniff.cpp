#include <stdio.h>
#include <iostream>
#include <bits/stdc++.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <pcap.h>
using namespace std;

struct sockaddr_in src, dst;
FILE *logfile;

int tcp = 0;

void handler(int sig){
    cout<<"\nTotal number of tcp packets caught = "<<tcp<<endl;
    SIG_DFL(SIGINT);
}

void handlePackets(u_char *args, const struct pcap_pkthdr *header, const u_char *buffer)
{
    // extracting the ip header from the packet
    struct iphdr *iph = (struct iphdr *)(buffer + sizeof(struct ethhdr));

    int protocol = iph->protocol;
    // catching only tcp packets
    if (protocol == 6)
    {
        unsigned short iphdrlen;
         
        struct iphdr *iph = (struct iphdr *)(buffer  + sizeof(struct ethhdr) );
        iphdrlen =iph->ihl*4;
        
        memset(&src, 0, sizeof(src));
        src.sin_addr.s_addr = iph->saddr;
        
        memset(&dst, 0, sizeof(dst));
        dst.sin_addr.s_addr = iph->daddr;
        
        fprintf(logfile , "\n");
        fprintf(logfile , "IP Header\n");
        fprintf(logfile , "   |-IP Version        : %d\n",(unsigned int)iph->version);
        fprintf(logfile , "   |-IP Header Length  : %d DWORDS or %d Bytes\n",(unsigned int)iph->ihl,((unsigned int)(iph->ihl))*4);
        fprintf(logfile , "   |-Type Of Service   : %d\n",(unsigned int)iph->tos);
        fprintf(logfile , "   |-IP Total Length   : %d  Bytes(Size of Packet)\n",ntohs(iph->tot_len));
        fprintf(logfile , "   |-Identification    : %d\n",ntohs(iph->id));
        fprintf(logfile , "   |-TTL      : %d\n",(unsigned int)iph->ttl);
        fprintf(logfile , "   |-Protocol : %d\n",(unsigned int)iph->protocol);
        fprintf(logfile , "   |-Checksum : %d\n",ntohs(iph->check));
        fprintf(logfile , "   |-Source IP        : %s\n" , inet_ntoa(src.sin_addr) );
        fprintf(logfile , "   |-Destination IP   : %s\n" , inet_ntoa(dst.sin_addr) );

        tcp++;
    }
}

int main(int argc, char *argv[])
{
    // setting the interrupt handler
    signal(SIGINT, handler);

    // device is set to the first command line argument
    char *device = argv[1];

    // opening the mentioned device for sniffing
    // max len of bytes captured set to 65535
    char *err;
    cout << "The device selected is: " << device<<endl;
    pcap_t *devHandle = pcap_open_live(device, 65535, 1, 1000, err);
    if (devHandle == NULL)
    {
        perror("pcap_open_live");
        cout << err;
        exit(EXIT_FAILURE);
    }

    // open a log file to dump the packets
    logfile = fopen("log.txt", "w");
    if (logfile == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    // start the sniffing
    pcap_loop(devHandle, -1, handlePackets, NULL);
}