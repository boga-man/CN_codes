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
using namespace std;

int main(){
    // socket server
    cout<<getpid()<<endl;
    cout<<"[ds4] - Data Server 4 started\n";
    int PORT = 8084;
    int sfd, nsfd, valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    strcpy(buffer, "Hello from Dataserver 4\n");
    int opt = 1;

    // creating a socket endpoint
    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("[ds4] - socket failed");
        exit(EXIT_FAILURE);
    }

    // set socket options
    if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("[ds4] - setsockopt");
        exit(EXIT_FAILURE);
    }

    // configure the socket address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // bind the address to the socket endpoint
    if (bind(sfd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("[ds4] - bind failed");
        exit(EXIT_FAILURE);
    }

    // listen for connection
    // we will have only one connection incoming and that will be of the multi data serer
    if (listen(sfd, 3) < 0)
    {
        perror("[ds4] - listen");
        exit(EXIT_FAILURE);
    }

    // accept the connection
    // accepting the multi data server connection
    cout<<"[ds4] - Waiting for connection\n";
    if ((nsfd = accept(sfd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    cout<<"[ds4] - Multi data server connected, waiting for request"<<endl;
    // continuously send the data to the multi data server on its request
    while (1)
    {
        // read the data from the multi data server
        char c;
        valread = read(nsfd, &c, 1);
        // if the data is not empty
        if (valread > 0)
        {
            // send the data to the multi data server
            write(nsfd, buffer, strlen(buffer));
        // if the data is empty, then terminate this data server
        }else{
            break;
        }
    }

    close(nsfd);
    close(sfd);
    return 0;
}