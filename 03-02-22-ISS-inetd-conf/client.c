#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <semaphore.h>
#include <fcntl.h>
int sendToServer(int nsfd, char *message)
{
    send(nsfd, message, strlen(message), 0);
}
int recieveFromServer(int nsfd, char *buffer)
{
    read(nsfd, buffer, 128);
}
char *recieveAndPrintFromServer(int nsfd)
{
    char *buff = (char *)calloc(200, sizeof(char));
    recieveFromServer(nsfd, buff);
    printf("Recived from server:\n'%s'\n", buff);
    fflush(stdout);
    return buff;
}
int recieveMenuFromServer(int nsfd)
{
    recieveAndPrintFromServer(nsfd);
}
int sendMenuResponse(int nsfd)
{
    char *response = (char *)calloc(200, sizeof(char));
    scanf("%s", response);
    printf("Sending %s\n", response);
    sendToServer(nsfd, response);
    int returnVal = atoi(response);
    if (returnVal >= 1 && returnVal <= 3)
        return returnVal;
    else
        return -1;
}
char *sendInputToServer(int nsfd)
{
    printf("Enter string to send to server:\n");
    char *buff = (char *)calloc(200, sizeof(char));
    scanf("%s", buff);
    buff[strlen(buff)] = '\n';
    sendToServer(nsfd, buff);
    printf("Sent '%s' to server\n", buff);
    return buff;
}
void recieveServiceAnsFromServer(int nsfd)
{
    recieveAndPrintFromServer(nsfd);
}
void printSockName(int sFD)
{
    struct sockaddr_in address;
    int addLen = sizeof(address);
    if (getsockname(sFD, (struct sockaddr *)&address, &addLen) == -1)
    {
        perror("getsockname() failed\n");
        exit(0);
    }
    printf("Local IP address is :%s\n", inet_ntoa(address.sin_addr));
    printf("Local port is :%d\n", (int)ntohs(address.sin_port));
}
void printPeerName(int nsFD)
{
    struct sockaddr_in address;
    int addLen = sizeof(address);
    if (getpeername(nsFD, (struct sockaddr *)&address, &addLen) == -1)
    {
        perror("getpeername() failed\n");
        exit(0);
    }
    printf("Foreign IP address is :%s\n", inet_ntoa(address.sin_addr));
    printf("Foreign port is :%d\n", (int)ntohs(address.sin_port));
}
int tcp(int argc, char *argv[])
{
    int SERVERPORT = atoi(argv[2]);
    int sFD;
    int nsFD;
    if ((sFD = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Error in creating a socket\n");
        exit(0);
    }
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SERVERPORT);
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    if (connect(sFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        perror("Connection error\n");
        exit(0);
    }
    
    bool accepted = false;
    char *responseChar;
    
    printf("Now enter input to service:\n");
    while (1)
    {
        char *buff = sendInputToServer(sFD);
        if (strcmp(buff, "exit\n") == 0)
        {
            printf("Exiting...\n");
            break;
        }
        free(buff);
        buff = recieveAndPrintFromServer(sFD);
        free(buff);
    }
}
void udp(int argc, char *argv[])
{

    int sfd;
    struct sockaddr_in serv_addr;
    int port_no = atoi(argv[2]);
    char buffer[256];

    bzero(&serv_addr, sizeof(serv_addr));

    if ((sfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        perror("\n socket");
    else
        printf("\n socket created successfully\n");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port_no);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    socklen_t serv_len = sizeof(serv_addr);
    bzero(buffer, sizeof(buffer));
    recvfrom(sfd, buffer, 256, 0, (struct sockaddr *)&serv_addr, &serv_len);
    printf("Recieved from server:%s\n", buffer);
}
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Need 3 arguments ./client <tcp/udp> <port_no>\n");
        exit(0);
    }
    if (strcmp(argv[1], "tcp") == 0)
    {
        tcp(argc, argv);
    }
    else
    {
        udp(argc, argv);
    }
}