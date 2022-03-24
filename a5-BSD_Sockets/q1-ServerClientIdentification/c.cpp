// #include <stdio.h>
// #include <iostream>
// #include <fcntl.h>
// #include <stdlib.h>
// #include <sys/types.h>
// #include <sys/wait.h>
// #include <sys/stat.h>
// #include <sys/shm.h>
// #include <sys/msg.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <arpa/inet.h> 
// #include <string.h>
// #include <signal.h>
// #include <unistd.h>
// #include <semaphore.h>
// using namespace std;

// int main(){
//     // socket client
//     int PORT = 8080;
//     int sfd, valread;
//     struct sockaddr_in address;
//     char buffer[1024] = {0};

//     // Creating socket file descriptor
//     if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
//     {
//         perror("socket failed");
//         exit(EXIT_FAILURE);
//     }
//     cout<<"Socket created"<<endl;

//     // setting up the address for communication
//     address.sin_family = AF_INET;
//     address.sin_port = htons(PORT);
//     if(inet_pton(AF_INET, "127.0.0.1", &address.sin_addr)<=0)
//     {
//         perror("inet_pton");
//         exit(EXIT_FAILURE);
//     }

//     cout<<"Connecting to server..."<<endl;
//     // connect the client socket to server socket
//     while(connect(sfd, (struct sockaddr *)&address, sizeof(address)) < 0)
//     {
//         perror("connect");
//         cout<<"Connection failed, retrying in 5 seconds"<<endl;
//         sleep(5);
//     }
//     cout<<"Connection has been established...\n";

//     // communicating with the server
//     string s;
//     getline(cin, s);
//     strcpy(buffer, s.c_str());
//     send(sfd, buffer, s.size(), 0);
//     memset(buffer, 0, 1024);

//     // closing the socket endpoint
//     close(sfd);

//     return 0;
// }


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
    // socket client
    int PORT = 8080;
    int sfd, valread;
    struct sockaddr_in address;
    char buffer[1024] = {0};

    // creating a socket endpoint
    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
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

    // set the socket options
    cout << "Connecting to the server ..." << endl;

    // continuously trying to connect the client socket to server socket
    while (connect(sfd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("connect");
        cout << "Failed, retrying in 5 seconds" << endl;
        sleep(5);
    }
    cout << "Connection established...\n";

    string s;
    cout << "Enter text to send to the server: ";
    getline(cin, s);
    strcpy(buffer, s.c_str());
    send(sfd, buffer, s.size(), 0);

    close(sfd);

    return 0;
}