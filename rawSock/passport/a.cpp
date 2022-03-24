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
#include "myHeader.h"
using namespace std;
#define BUF_SIZE 1024

int main(){
    // connect to the uds socket of s
    int usfd = ConnectToUdsServer(UDS_SERVER_PATH);


    // recieve the rsfd from server
    int rsfd = recv_fd(usfd);
    // broadcast required documents to rsfd
    struct sockaddr_in client;
	client.sin_family=AF_INET;
	client.sin_addr.s_addr=inet_addr("127.0.0.1");
    cout<<"Enter the required documents for verification: ";
    string reqDocs;
    cin>>reqDocs;
    sendto(rsfd, reqDocs.c_str(), reqDocs.size(), 0, (struct sockaddr*)&client, sizeof(client));
    // send the rsfd back to server
    send_fd(usfd, rsfd);
    // close rsfd
    close(rsfd);

    char buf[1024] = {0};
    // recieve the client nsfd from server for verification
    while(1){
        // receive the nsfd from server
        int nsfd = recv_fd(usfd);
        cout<<"CLIENT: ";
        // reading the documents
        read(nsfd, buf, BUF_SIZE);
        cout<<buf<<endl;
        // send the nsfd back to server
        send_fd(usfd, nsfd);
        // close nsfd
        close(nsfd);
        // print the verdict
        cout<<"Enter VERDICT: ";
        string ver;
        cin>>ver;
        cout<<ver<<endl;

        memset(buf, 0, BUF_SIZE);
    }
}