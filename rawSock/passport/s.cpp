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

vector<int> nusfds;
vector<int> nsfds;

int main(){
    // initiate tcp server
    int sfd = TcpServerInit(8000);

    // initiate uds server
    int usfd = UdsServerInit(UDS_SERVER_PATH);
    listen(usfd, 10);

    // accept conections of A,B,C
    for(int i = 0; i<3; i++){
        int nusfd = accept(usfd, NULL, NULL);
        nusfds.push_back(nusfd);
    }
    cout<<"A B C connected to server\n";

    // create raw socket to connect to broadcast documents
    int rsfd = RawSocketInit(2);
    cout<<"Raw socket created\n";

    // send the rsfd to all a,b,c
    for(int i = 0; i<3; i++){
        send_fd(nusfds[i], rsfd);
        recv_fd(nusfds[i]);
    }
    cout<<"rsfd sent to A B C\n";

    // listen on tcp socket for clients
    listen(sfd, 10);
    cout<<"Listening on tcp socket\n";
    // accept client connections 
    while(1){
        int nsfd = accept(sfd, NULL, NULL);
        nsfds.push_back(nsfd);
        cout<<"Client connected for verification\n";
        // send the nsfd to all a,b,c
        for(int i = 0; i<3; i++){
            send_fd(nusfds[i], nsfd);
            recv_fd(nusfds[i]);
        }
    }
}