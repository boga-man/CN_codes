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
    int i = 3, entered = 0;
    do
    {
        if ((entered = connect(sfd, (struct sockaddr *)&saddr, sizeof(saddr))) == -1)
        {
            perror("connect");
            cout << "Retrying to enter the station\n";
            i--;
        }
        sleep(2);
    }while(entered==-1);

    if (entered==-1)
    {
        cout << "Time out, exiting\n";
        return 0;
    }

    cout << "Entered the station\n";

    string msg = "I am sending my compartments\n";
    send(sfd, msg.c_str(), msg.size(), 0);

    // simulating passing through platform
    sleep(5);

    cout<<"Left the station. Bye!\n";

    return 0;
}