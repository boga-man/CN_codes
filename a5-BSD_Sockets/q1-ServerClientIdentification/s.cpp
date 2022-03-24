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

int main()
{
    int PORT = 8080;
    int sfd, nsfd, valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    int opt = 1;

    // creating a socket endpoint
    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // set the socket options
    if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    cout << "Created the socket" << endl;

    // setting the address config
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // binding the address to the socket endpoint
    if (bind(sfd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    cout << "Binded socket to address.." << endl;

    // getting this socket enpoint details
    getsockname(sfd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    char localIP[16];
    // converting the address from binary to characters
    inet_ntop(AF_INET, &address.sin_addr, localIP, sizeof(localIP));
    // converting the port from binary to int
    int myPort = ntohs(address.sin_port);
    cout << "Local IP: " << localIP << endl;
    cout << "Local Port: " << myPort << endl;

    // listen to the socket, with maximum buffer for clients
    if (listen(sfd, SOMAXCONN) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    cout << "Socket initiated listening at port - " << PORT << endl;

    int noOfConn = 0; 
    while (1)
    {
        // accept the incoming connection and get nsfd
        if ((nsfd = accept(sfd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        noOfConn++;
        cout << "Received and accepted the connection number: "<<noOfConn<<endl;

        // getting the client details
        getpeername(nsfd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        char foreignIP[16];
        // converting the address from binary to char
        inet_ntop(AF_INET, &address.sin_addr, foreignIP, sizeof(foreignIP));
        // converting the port from binary to int
        int cPort = ntohs(address.sin_port);
        cout << "Client IP: " << foreignIP << endl;
        cout << "Client Port: " << cPort << endl;

        // read from client
        valread = read(nsfd, buffer, 1024);
        string su(buffer);
        if(valread == 0){
            cout<<"Connection destroyed before proper communication\n";
            close(nsfd);
            continue;
        }
        cout << "Client sent: " << buffer << "\n-------------------------------------------------------------------" << endl;
        memset(buffer, 0, 1024);

        // closing the connection to make the server available for other clients
        close(nsfd);
    }

    // close the sockets
    close(sfd);
    return 0;
}