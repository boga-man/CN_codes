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
#include "fd_passing.h"
using namespace std;

int main(){
    // tcp client
    int sfd;
    struct sockaddr_un caddr_un;
    char buf[1024];
    int opt = 1;
    int PORT;
    cout<<"Enter the port number: ";
    cout<<"Available ports: 3001; 3002, 3003, ...\n";
    cin>>PORT;

    string client_socket_path = "/tmp/client_socket";

    // creating the unix socket
    if((sfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){
        perror("socket");
        exit(1);
    }

    // setup the caddr_un structure to connect to 127.0.0.1
    memset(&caddr_un, 0, sizeof(sockaddr_un));
    caddr_un.sun_family = AF_UNIX;
    strcpy(caddr_un.sun_path, client_socket_path.c_str());

    // connect to the server
    if(connect(sfd, (struct sockaddr*)&caddr_un, sizeof(caddr_un)) == -1){
        perror("connect");
        exit(1);
    }

    // receive the train fd and communicate with the train
    while(1){
        // receive train from the station master of a train arrival
        char msg[100] = {0};
        if(read(sfd, msg, strlen(msg)) == 0){
            perror("send");
            exit(1);
        }
        cout<<msg<<endl;

        // receiving the train fd from station master
        int tsfd = recv_fd(sfd);

        // receive compartments from train
        
    }    

    return 0;

}