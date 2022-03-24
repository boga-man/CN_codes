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

int sfds[100];
int sid[100];
int readPipefd[100];

struct thrd_data{
    int servNum;
    int nsfd;
};

void CreateBindListenTCP(int port, int sfds[], int &noOfServ){
    // create the socket for this service
    cout<<"Entered the function\n";
    if((sfds[noOfServ]=socket(AF_INET, SOCK_STREAM, 0))==-1){
        perror("socket creation");
        exit(1);
    }

    // set socket options
    int opt = 1;
    if(setsockopt(sfds[noOfServ], SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))==-1){
        perror("socket options");
        exit(1);
    }

    // binding this socket to an address
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(3000);

    if ( bind(sfds[noOfServ], (const struct sockaddr *)&addr, sizeof(addr)) < 0 )
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // listen for request
    if ((listen(sfds[noOfServ], 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }
    
    return;
}

int mypopen(string path){
    int pfd[2];
    int pid = fork();
    if(pid == 0){
        // child process - only writes to the pipe
        // so close the reading end
        close(pfd[0]);
        dup2(pfd[1], 1);
        execv(path.c_str(), NULL);
    }else{
        // parent process - only reads from the pipe
        // so close writing end
        close(pfd[1]);
        // return the pipe reading end to store
        return pfd[0];
    }
}

void* reqHandler(void *arg){
    // getting the service number from arguments
    int servNum = *(int *)arg;

    // read from the service
    char buff[1024]={0};
    
    // signal the service
    kill(sid[servNum], SIGUSR1);

    // read from the service
    if(read(readPipefd[servNum], buff, 1024)<=0){
        perror("read");
        exit(1);
    }

    // send this to the client
    send(sfds[servNum], buff, 1024, 0);

    // done servicing the client
}

int main(){
    // First start the UDP connection
    // initiate the socket
    int sfd;
    int noOfServ = 0;
    struct sockaddr_in saddr;
    if((sfd=socket(AF_INET, SOCK_DGRAM, 0))==-1){
        perror("socket creation");
        exit(1);
    }

    // set socket options
    int opt = 1;
    if(setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))==-1){
        perror("socket options");
        exit(1);
    }
    
    // bind the socket to an address
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = htons(3000);
    if ( bind(sfd, (const struct sockaddr *)&saddr, sizeof(saddr)) < 0 )
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // There is no listening required for UDP

    // Now take the input from user to start the services
    cout<<"Enter the service to start\nInput format: PORT_NUMBER /pathname/Si.exe\n";
    cout<<"Enter -1 to stop creation of services\n";
    cout<<"Note: Number of services can't be greater than 100\n";
    int portNum=0;
    string servPath;
    // creates the service process and stores its read pipe end to readPipefd, and its corresponding sfd to sfds[100]
    cin>>portNum>>servPath;
    while(portNum!=-1){        
        CreateBindListenTCP(portNum, sfds, noOfServ);

        // fork the services and create a pipe connection
        // using popen for convinience
        readPipefd[noOfServ] = mypopen(servPath);

        // increase the number of services
        noOfServ++;

        cin>>portNum>>servPath;
    }

    // poll on all the sfds created for requests 
    struct pollfd pfds[100];
    // add all the service sfds to poll
    for(int i = 0; i<noOfServ; i++){
        pfds[i].fd = sfds[i];
        pfds[i].events = POLLIN;
        pfds[i].revents = 0;
    }
    
    // poll on the sfds and handle the requests 
    while(1){
        // timeout 1000ms
        int ready = poll(pfds, noOfServ, 1000);
        pthread_t tid;
        // check which are set and perform appropriate task
        for(int i = 0; i<noOfServ; i++){
            if(pfds[i].revents & POLLIN){
                struct thrd_data *data = new thrd_data();
                int len = sizeof(saddr);
                data->servNum = i;
                data->nsfd = accept(pfds[i].fd, (struct sockaddr *)&saddr, (socklen_t *)&len);
                pthread_create(&tid, NULL, reqHandler, (struct thrd_data *)&data);
            }
        }
    }
}