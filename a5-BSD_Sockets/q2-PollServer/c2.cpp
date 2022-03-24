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
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <semaphore.h>
using namespace std;
int main()
{
    int PORT = 8081;
    int socketFD, valread;
    struct sockaddr_in address;
    char buffer[1024] = {0};
    // Creating socket file descriptor
    if ((socketFD = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    cout << "Socket created" << endl;
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &address.sin_addr) <= 0)
    {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }
    cout << "Trying to connect to server" << endl;
    while (connect(socketFD, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        cout << "connection falied, waiting for 5 seconds" << endl;
        sleep(5);
    }
    cout << "connected to server" << endl;
    struct sockaddr_in serverAddress;
    socklen_t addrlen = sizeof(address);
    
    string s;
    cin >> s;
    strcpy(buffer, s.c_str());
    send(socketFD, buffer, s.size(), 0);
    memset(buffer, 0, 1024);
    read(socketFD, buffer, 1024);
    cout << "Server: " << buffer << endl;
    close(socketFD);
    return 0;
}