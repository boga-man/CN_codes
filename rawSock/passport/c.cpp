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
#include <bits/stdc++.h>
#include <sys/un.h>
#include <netinet/ip.h>
#include "myHeader.h"

using namespace std;

#define BUF_SIZE 1024

int main(){
    // client first receives broadcast from the server raw socket
    int rsfd = RawSocketInit(2);
    cout<<"Raw socket initiated"<<endl;

    // receive the broadcast by the server
    char buf[1024] = {0};
    for(int i = 0; i<3; i++){
        read(rsfd, buf, BUF_SIZE);
        struct iphdr *ip;
        ip=(struct iphdr*)buf;
        cout<<(buf+(ip->ihl)*4)<<endl;
    }

    // client then sends the required documents to the server tcp socket
    // connect to the server tcp socket
    int sfd = ConnectToTcpServer("127.0.0.1", 8000);
    // send the required documents to the server three times (a,b,c)
    cout<<"Enter the required documents: ";
    for(int i = 0; i<3; i++){
        string reqDocs;
        cin>>reqDocs;
        send(sfd, reqDocs.c_str(), reqDocs.size(), 0);
    }

}