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
#include <bits/stdc++.h>
#include <arpa/inet.h>
using namespace std;

int dsfd[5] = {0};

struct thread_data{
    int nsfd;
    int sfd;
    char dsNumC;
};

void initDataServers(){
    system("g++ d1.cpp -o d1");
    system("g++ d2.cpp -o d2");
    system("g++ d3.cpp -o d3");
    system("g++ d4.cpp -o d4");
}

void *handleClient(void *arg){
    struct thread_data data = *(struct thread_data *)arg;
    cout<<"[master] - In the new thread\n";
    // read the dataserver number from the client
    int clientsfd = data.nsfd;
    int dsNum = (int)(data.dsNumC - '0');
    char dsNumC = data.dsNumC;

    string dsName = "d" + to_string(dsNum);
    string executable = "./" + dsName;

    // creating a client socket
    cout<<dsfd[dsNum]<<" "<<dsNum<<endl;
    if(dsfd[dsNum] == 0){
        // create a socket and connect it to requested data server only if there was no previous connection available to this data server
        if ((dsfd[dsNum] = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        {
            perror("[master] - socket failed");
            exit(EXIT_FAILURE);
        }
        // connect to the dataserver dsNum
        struct sockaddr_in address;
        int addrlen = sizeof(address);
        
        address.sin_family = AF_INET;
        address.sin_port = htons(8080 + dsNum);
        if (inet_pton(AF_INET, "127.0.0.1", &address.sin_addr) <= 0)
        {
            perror("[master] - inet_pton");
            exit(EXIT_FAILURE);
        }
        // connecting to the data server
        cout<<"[master] - Connecting to the data server "<<dsNum<<endl;
        if(connect(dsfd[dsNum], (struct sockaddr *)&address, addrlen) < 0){
            cout<<"[master] - Data server is not running, initiating the dataserver "<<dsNum<<endl;
            int pid = fork();
            if(pid == 0){
                close(clientsfd);
                close(data.sfd);
                // child process to execute dataserver
                execv(executable.c_str(), NULL);
            }else{
                // connect to the data server
                sleep(1);
                connect(dsfd[dsNum], (struct sockaddr *)&address, sizeof(address));
            }
        }
    }

    // send the message(request) to the dataserver
    cout<<"[master] - Communicating with the dataserver "<<dsNum<<"\n";
    send(dsfd[dsNum], &dsNumC, 1, 0);

    // receive the response from the dataserver
    char buffer[1024] = {0};
    recv(dsfd[dsNum], buffer, 1024, 0);

    // send this data to the client that connected to this server
    send(clientsfd, buffer, 1024, 0);
    close(clientsfd);
    cout<<"[master] - closing the thread\n";
    return NULL;
}

int main()
{
    initDataServers();
    int PORT = 3001;
    int sfd, nsfd, valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    int opt = 1;


    // creating a socket endpoint
    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("[master] - socket failed");
        exit(EXIT_FAILURE);
    }

    // set the socket options
    if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("[master] - setsockopt");
        exit(EXIT_FAILURE);
    }

    // setting the address config
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // binding the address to the socket endpoint
    if (bind(sfd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("[master] - bind failed");
        exit(EXIT_FAILURE);
    }

    // listen at the socket, with maximum buffer for clients
    if (listen(sfd, SOMAXCONN) < 0)
    {
        perror("[master] - listen");
        exit(EXIT_FAILURE);
    }
    cout << "[master] - Socket endpoint initiated listening at port - " << PORT << endl;

    pthread_t client_thread;
    while (1)
    {
        cout<<"[master] - Waiting for a client\n";
        // accept the incoming connection and get nsfd
        if ((nsfd = accept(sfd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("[master] - accept");
            exit(EXIT_FAILURE);
        }

        cout<<"[master] - Connected to a client\n";
        // read a character from the client
        char ds;
        int valread = read(nsfd, &ds, 1);
        if(valread == 0){
            cout<<"[master] - Connection destroyed before proper communication\n";
            close(nsfd);
            return NULL;
        }
        int dsNum = ds - '0';
        struct thread_data data;
        data.nsfd = nsfd;
        data.dsNumC = ds;

        cout<<"[master] - Request for the data server "<< ds <<" from a client"<<endl;

        // handling this request via a thread
        pthread_create(&client_thread, NULL, handleClient, &data);       
    }

    // close the sockets
    close(sfd);
    return 0;
}