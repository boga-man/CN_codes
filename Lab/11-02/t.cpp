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

int main()
{
    // tcp client
    int sfd;
    struct sockaddr_in saddr;
    char buf[1024];
    int opt = 1;
    int len = sizeof(saddr);
    int PORT;
    cout << "Enter the port of this train\n";
    cin >> PORT;

    // create socket
    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    // set socket options
    if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1)
    {
        perror("setsockopt");
        exit(1);
    }

    // setup the saddr structure to connect to 127.0.0.1
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    saddr.sin_port = htons(PORT);

    // connect to the server
    int i = 5, entered = 0;
    while (i)
    {
        if (connect(sfd, (struct sockaddr *)&saddr, sizeof(saddr)) == -1)
        {
            perror("connect");
            cout << "Platform not available\n";
        }
        i--;
        cout << "Retrying to enter the station\n";
        sleep(2);
    }

    if (!entered)
    {
        cout << "Time out, exiting\n";
        return 0;
    }

    cout << "Entered the station\n";

    string msg = "Engine entering the station\n";
    if(send(sfd, msg.c_str(), 1024, 0)<=0){
        cout<<"Disconnected from the station\n";
        return 0;
    }

    // simulating passing along the platform
    sleep(2);

    msg = "Last compartment left the platform\n";
    if(send(sfd, msg.c_str(), 1024, 0) <= 0){
        cout<<"Disconnected from the station\n";
        return 0;
    }

    cout<<"Left the station. Bye!\n";

    return 0;
}