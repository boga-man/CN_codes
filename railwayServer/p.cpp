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
#include <netinet/in.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <bits/stdc++.h>
#include "fd_passing.h"
using namespace std;

int main(){
    // tcp client
    int sfd;
    struct sockaddr_un saddr_un;
    char buf[1024];
    int opt = 1;

    string server_socket_path = "./server_socket";

    // creating the unix socket
    if((sfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){
        perror("socket");
        exit(1);
    }

    // setup the saddr_un structure to connect to 127.0.0.1
    memset(&saddr_un, 0, sizeof(sockaddr_un));
    saddr_un.sun_family = AF_UNIX;
    strcpy(saddr_un.sun_path, server_socket_path.c_str());

    // connect to the server
    if(connect(sfd, (struct sockaddr*)&saddr_un, sizeof(saddr_un)) == -1){
        perror("connect");
        exit(1);
    }
    cout<<"Connected to the sm\n";

    // creating a thread for displaying ads
    // create a rsfd for receiving the ads
    cout<<"Starting the ad service\n";
    int rsfd = RawSocketInit(adProtoNum);
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, display_ads, (void*)&rsfd);

    // receive the train fd and communicate with the train
    while(1){

        // receive train from the station master of a train arrival
        cout<<"Waiting for a train\n";

        // receiving the train fd from station master
        int tsfd = recv_fd(sfd);

        // get the compartments from the train
        cout<<"Getting the compartments from the train\n";
        char buff[1024] = {0};
        recv(tsfd, buff, 1024, 0);
        cout<<buff<<endl;

        // receive compartments from train
        cout<<"Receiving compartments from the train\n";

        // simulating passing along the platform
        sleep(5);

        // sending back sfd
        send_fd(sfd, tsfd);
    }    

    return 0;

}