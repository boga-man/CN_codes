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
#include <semaphore.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <bits/stdc++.h>
#include <sys/un.h>
using namespace std;

int main(){
    int rsfd = socket(AF_INET, SOCK_RAW, 2);
    if(rsfd < 0){
        perror("socket");
        exit(1);
    }

    // send a message to the server
    while(1){
        cout<<"Enter a message: ";
        char msg[1024];
        cin.getline(msg, sizeof(msg));
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        sendto(rsfd, msg, 1024, 0, (struct sockaddr *)&addr, sizeof(addr));
    }
}