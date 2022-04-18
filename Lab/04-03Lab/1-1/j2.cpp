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
#include <sys/un.h>
using namespace std;
#define SERVER_PATH "./server"
#define CLIENT_PATH "./client"

int client_sock;

void askQues(int sig){
    cout<<"I am asking a question\n";
    string ques = "I am Judge2\nWhat is your name?\n";
    // send question to the screen
    send(client_sock, ques.c_str(), sizeof(ques), 0);
    // cout<<bytesSent;
    return;
}

void sendScore(int sig){
    string score = "Judge2 -> 95\n";
    // send score to the screen
    send(client_sock, score.c_str(), sizeof(score), 0);
    return;
}

int main(){
    // first connect to the Screen using Unix Domain Sockets
    int rc, len;
    int opt = 1;
    struct sockaddr_un usaddr; 
    struct sockaddr_un ucaddr; 
    char buf[256];
    memset(&usaddr, 0, sizeof(struct sockaddr_un));
    memset(&ucaddr, 0, sizeof(struct sockaddr_un));
    
    client_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_sock == -1) {
        perror("socket");
        exit(1);
    }

    ucaddr.sun_family = AF_UNIX;   
    strcpy(ucaddr.sun_path, CLIENT_PATH); 
    len = sizeof(ucaddr);
    
    unlink(CLIENT_PATH);
    rc = bind(client_sock, (struct sockaddr *) &ucaddr, len);
    if (rc == -1){
        perror("bind");
        close(client_sock);
        exit(1);
    }

    if(setsockopt(client_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))<0){
        perror("setsockopt");
        exit(2);
    }

    usaddr.sun_family = AF_UNIX;
    strcpy(usaddr.sun_path, SERVER_PATH);
    rc = connect(client_sock, (struct sockaddr *) &usaddr, len);
    if(rc == -1){
        perror("connect");
        close(client_sock);
        exit(1);
    }
    cout<<"Connected to screen\n";


    // setting up the signal handler
    signal(SIGUSR1, askQues);
    signal(SIGUSR2, sendScore);
    
    // // wait for SIGUSR1/SIGUSR2 from Anchor to ask question OR send score
    // // SIGUSR1 -> ask question
    // // SIGUSR2 -> send scores
    // pause();

    // // second signal will either end this process or sends score to screen
    // pause();
    while(1);
}