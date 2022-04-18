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
#define SOCK_PATH  "./server"

// for tcp
int sfd, nsfd;
int opt = 1;
struct sockaddr_in saddr;
int PORT = 3000;
pthread_t tid;

// for uds
int usfd, len, rc;
int bytes_rec = 0;
struct sockaddr_un server_sockaddr;
struct sockaddr_un client_sockaddr;     
int j1sfd, j2sfd, j3sfd;

int getPidFromPName(string name){
    string command = "pidof " + name;
    int fd = fileno(popen(command.c_str(), "r"));
    char s[1000];
    read(fd, &s, 1000);
    int pid = atoi(s);
    return pid;
}

void *performerThread(void *arg){
    int sfd = *(int *)arg;
    cout<<sfd;
    if (listen(sfd, 3) != 0)
    {
        perror("Listen()");
        exit(4);
    }

    struct sockaddr_in caddr;
    int len = sizeof(caddr);
    if((nsfd=accept(sfd, (struct sockaddr*)&caddr, (socklen_t *)&len))==-1){
        perror("accept");
        exit(5);
    }
    cout<<"Performer accepted\n";

    // Reading performance from P
    char buff[1024] = {0};
    read(nsfd, buff, 1024);
    cout<<"Performance of P:\n";
    cout<<buff<<endl;

    // signal A after the performance ends
    // ---------------------------------------------------
    string aName = "a";
    int apid = getPidFromPName(aName);
    kill(apid, SIGUSR1);

    // ---------------------------------------------------


    // Reading answer from performer
    memset(buff, 0, 1024);
    recv(nsfd, buff, 1024, 0);
    cout<<"Performer answered:\n";
    cout<<buff<<endl;

    // send signal to A that performer has answered and ask scores from judges
    kill(apid, SIGUSR2);

    return NULL;
}

void StartUds(){
    memset(&server_sockaddr, 0, sizeof(struct sockaddr_un));

    usfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (usfd == -1){
        perror("Unix socket");
        exit(1);
    }

    server_sockaddr.sun_family = AF_UNIX;   
    strcpy(server_sockaddr.sun_path, SOCK_PATH); 
    len = sizeof(server_sockaddr);
    
    unlink(SOCK_PATH);
    rc = bind(usfd, (struct sockaddr *) &server_sockaddr, len);
    if (rc == -1){
        perror("Unix bind");
        close(usfd);
        exit(1);
    }
    
    cout<<"Created and binded unix socket\n";
    rc = listen(usfd, 10);
    if (rc == -1){ 
        perror("Unix listen");
        close(usfd);
        exit(1);
    }
    printf("unix socket listening...\n");

    // accepting connections with judges
    j1sfd = accept(usfd, (struct sockaddr *) &client_sockaddr, (socklen_t *)&len);
    if (j1sfd == -1){
        perror("Unix accept");
        close(usfd);
        close(j1sfd);
        exit(1);
    }
    cout<<"Connected to judge1 via UDS\n";

    j2sfd = accept(usfd, (struct sockaddr *) &client_sockaddr, (socklen_t *)&len);
    if (j2sfd == -1){
        perror("Unix accept");
        close(usfd);
        close(j2sfd);
        exit(1);
    }
    cout<<"Connected to judge2 via UDS\n";

    j3sfd = accept(usfd, (struct sockaddr *) &client_sockaddr, (socklen_t *)&len);
    if (j3sfd == -1){
        perror("Unix accept");
        close(usfd);
        close(j3sfd);
        exit(1);
    }
    cout<<"Connected to judge3 via UDS\n";

    // wait for question from any one of the judges
    // use Poll
    // poll on all the sfds created for requests 
    struct pollfd pfds[100];
    // add all the service sfds to poll
    // pfds[0].fd = j1sfd;
    // pfds[0].events = POLLIN;
    // pfds[0].revents = 0;
    // pfds[1].fd = j2sfd;
    // pfds[1].events = POLLIN;
    // pfds[1].revents = 0;
    // pfds[2].fd = j3sfd;
    // pfds[2].events = POLLIN;
    // pfds[2].revents = 0;
    // cout<<"Polling for question from a judge\n";

    // int quesJudge;
    
    // // poll on the sfds and handle the requests 
    // while(1){
    //     // timeout 1000ms
    //     int ready = poll(pfds, 2, 1000);
    //     pthread_t tid;
    //     bool flag = false;
    //     // check which are set and perform appropriate task
    //     for(int i = 0; i<2; i++){
    //         if(pfds[i].revents & POLLIN){
    //             quesJudge = pfds[i].fd;
    //             flag = true;
    //             break;
    //         }
    //     }
    //     if(flag){
    //         break;
    //     }
    // }
    
    char ques[1024] = {0};
    recv(j2sfd, ques, 1024, 0);
    cout<<"Question from a judge: "<<endl;
    cout<<ques<<endl;

    // send ques to P
    send(nsfd, ques, 1024, 0);

    // receive scores from judges and display on screen
    char s1[1024], s2[1024], s3[1024];
    recv(j1sfd, s1, 1024, 0);
    recv(j2sfd, s2, 1024, 0);
    recv(j3sfd, s3, 1024, 0);

    cout<<"Score by Judge1: "<<s1<<endl;
    cout<<"Score by Judge2: "<<s2<<endl;
    cout<<"Score by Judge3: "<<s3<<endl;

    return;
    
}

void StartTcpSocket(){
    // creating the socket endpoint
    if((sfd=socket(AF_INET, SOCK_STREAM, 0))<0){
        perror("socket");
        exit(2);
    }
    cout<<"Tcp socket created\n";

    // setting socket options
    if(setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))<0){
        perror("setsockopt");
        exit(2);
    }

    saddr.sin_family = AF_INET;
    saddr.sin_port   = htons(PORT);
    saddr.sin_addr.s_addr = INADDR_ANY;

    // binding the socket
    if (bind(sfd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
    {
        perror("Bind()");
        exit(3);
    }
    cout<<"Tcp socket binded\n";

    pthread_create(&tid, NULL, performerThread, &sfd);
    return;
}


int main(){
    cout<<"Starting tcp\n";
    // Start tcp socket to communicate with the client
    StartTcpSocket();

    // Start uds to communicate with the judges
    StartUds();
    pthread_join(tid, NULL);
}