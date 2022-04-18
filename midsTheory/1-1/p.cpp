#include <stdio.h>
#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
using namespace std;

int main(){
    int sfd, nsfd;
    int opt = 1;
    struct sockaddr_in saddr;
    int PORT = 3000;

    // creating the socket endpoint
    if((sfd=socket(AF_INET, SOCK_STREAM, 0))<0){
        perror("socket");
        exit(2);
    }

    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    saddr.sin_port = htons(PORT);
   
    // connect the client socket to server socket
    if (connect(sfd, (struct sockaddr*)&saddr, sizeof(saddr)) != 0) {
        perror("connect");
        exit(1);
    }

    string s;
    cin>>s;
    // sending performance to Screeen
    send(sfd, s.c_str(), sizeof(s), 0);

    // receive question from screen
    char ques[1024] = {0};
    recv(sfd, ques, 1024, 0);
    cout<<"Question from judges\n";
    cout<<ques<<endl;

    // send answer to screen
    cin>>s;
    send(sfd, s.c_str(), sizeof(s), 0);

    return 0;
}