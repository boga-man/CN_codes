#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/poll.h>

struct service_conf_t
{
    char serviceName[100];
    char sockType[100];
    char protName[100];
    bool wait;
    char userName[100];
    char serverPath[100];
    char serverArgs[100];
};

typedef struct service_conf_t service_conf;

void printConf(service_conf *conf)
{
    printf("Serive name:%s\n", conf->serviceName);
    printf("Socket type:%s\n", conf->sockType);
    printf("ProtName: %s\n", conf->protName);
    printf("wait:%d\n", conf->wait);
    printf("user name:%s\n", conf->userName);
    printf("server path:%s\n", conf->serverPath);
    printf("server args:%s\n", conf->serverArgs);
}

struct service_conf_t *getServiceConf(char *line)
{
    char *temp = strtok(line, " \n\t");
    struct service_conf_t *currConf = (service_conf *)malloc(sizeof(service_conf));
    strcpy(currConf->serviceName, temp);
    temp = strtok(NULL, " \t\n");
    strcpy(currConf->sockType, temp);
    temp = strtok(NULL, " \t\n");
    strcpy(currConf->protName, temp);
    temp = strtok(NULL, " \t\n");
    if (strcmp(temp, "wait") == 0)
        currConf->wait = true;
    else
        currConf->wait = false;
    temp = strtok(NULL, " \t\n");
    strcpy(currConf->userName, temp);
    temp = strtok(NULL, " \t\n");
    strcpy(currConf->serverPath, temp);
    temp = strtok(NULL, " \t\n");
    printf("Here\n");
    printf("temp:%s\n", temp);
    if (temp != NULL)
        strcpy(currConf->serverArgs, temp);
    else
        strcpy(currConf->serverArgs, "");
    return currConf;
}

service_conf **readConfigurations(const char *fileName, int *size)
{
    service_conf **allConfs = (service_conf **)calloc(100, sizeof(struct service_conf_t *));
    FILE *f = fopen(fileName, "r");
    char currLine[500];
    char *returnStr = fgets(currLine, 500, f);
    while (returnStr != NULL)
    {
        allConfs[(*size)++] = getServiceConf(currLine);
        returnStr = fgets(currLine, 500, f);
    }
    return allConfs;
}

int sendToClient(int nsfd, char *message)
{
    send(nsfd, message, strlen(message), 0);
}
int recieveFromClient(int nsfd, char *buffer)
{
    read(nsfd, buffer, 128);
}
char *recieveAndPrintFromClient(int nsfd)
{
    char *buff = (char *)calloc(200, sizeof(char));
    recieveFromClient(nsfd, buff);
    printf("Recived from client:%s\n", buff);
    fflush(stdout);
    return buff;
}
void sendMenuToClient(int nsfd)
{
    char *message = (char *)calloc(200, sizeof(char));
    sprintf(message, "1)Echo Service\n2)Small to captial\n3)Captial to small\n4)Exit\n");
    sendToClient(nsfd, message);
    free(message);
}
int recieveMenuResponse(int nsfd)
{
    char *buffer = (char *)calloc(200, sizeof(char));
    recieveFromClient(nsfd, buffer);
    int response = atoi(buffer);
    printf("Recieved from client as menu response:'%s'\n", buffer);
    free(buffer);
    if (response >= 1 && response <= 4)
        return response;
    else
        return -1;
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

void *handleFunc(void *arrPtr)
{
    int *arr = (int *)arrPtr;
    fflush(stdout);
    int nsFD = arr[0];
    int response = arr[1];
    {
        if (fork() == 0)
        {
            char *arg[3][2] = {{"./echoService", NULL}, {"./smallToCapital", NULL}, {"./capitalToSmall", NULL}};
            fflush(stdout);
            dup2(nsFD, STDIN_FILENO);
            dup2(nsFD, STDOUT_FILENO);
            execvp(arg[response - 1][0], arg[response - 1]);
        }
    }
}

void bindAndListenTcp(int portNo, int *sFDPtr)
{
    printf("Trying to create sfd\n");
    if ((*sFDPtr = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Error in creating a socket\n");
        exit(0);
    }
    int True = 1;
    if (setsockopt(*sFDPtr, SOL_SOCKET, SO_REUSEADDR, &True, sizeof(int)) < 0)
    {
        printf("Error in sock option errorno:%d\n error:%s", errno, strerror(errno));
        exit(0);
    }
    printf("sFD creation successful\n");
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(portNo);

    printf("Trying to bind the address\n");
    if (bind(*sFDPtr, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        printf("Error in bind, error no:%d, error is: %s\n", errno, strerror(errno));
        exit(0);
    }
    printf("Bind successful\n");
    printSockName(*sFDPtr);
    printf("Listening for connections\n");
    if (listen(*sFDPtr, 10) < 0)
    {
        perror("Error in listen\n");
        exit(0);
    }
    printf("Server started listening\n");
}

struct ipAddStruct_t
{
    char *ipAdd;
    char *portNo;
};
typedef struct ipAddStruct_t ipAddStruct;

struct sockaddr_in getSockAdd(ipAddStruct *ipAdd)
{
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(ipAdd->portNo));
    serverAddr.sin_addr.s_addr = inet_addr(ipAdd->ipAdd);
    return serverAddr;
}

void bindUdp(ipAddStruct *ipAdd, int *sFDPtr)
{
    if ((*sFDPtr = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        printf("Error in udp socket creation\n");
        exit(0);
    }
    struct sockaddr_in serverAddr = getSockAdd(ipAdd);
    if ((bind(*sFDPtr, (struct sockaddr *)&serverAddr, sizeof(serverAddr))) == -1)
    {
        printf("Error in udp bind\n");
        printf("error:%s errorno:%d\n", strerror(errno), errno);

        exit(EXIT_FAILURE);
    }
    printf("Bind successful\n");
}

void acceptHandler(int sFD, service_conf *currConf)
{
    if (strcmp(currConf->protName, "tcp") == 0)
    {
        int nsFD;
        struct sockaddr_in serverAddress;
        int addressLength = sizeof(serverAddress);
        if ((nsFD = accept(sFD, (struct sockaddr *)&serverAddress, &addressLength)) < 0)
        {
            perror("Error in accept\n");
            exit(0);
        }
        if (fork() != 0)
        {
            close(nsFD);
        }
        else
        {
            close(sFD);
            dup2(nsFD, STDOUT_FILENO);
            dup2(nsFD, STDIN_FILENO);
            execl(currConf->serverPath, currConf->serverArgs);
        }
    }
    else
    {

        printf("in udp\n");
        char buff[256] = "from udp";
        write(sFD, buff, 256);
    }
}

int main(int argc, char const *argv[])
{
    int nsFD;
    int size = 0;
    service_conf **allConf = readConfigurations("test.txt", &size);
    int portNos[size];
    int sFDs[size];
    for (int i = 0; i < size; i++)
    {
        portNos[i] = 50500 + i;
        if (strcmp(allConf[i]->protName, "tcp") == 0)
            bindAndListenTcp(portNos[i], &sFDs[i]);
        else
        {
            ipAddStruct currIpAdd;
            currIpAdd.ipAdd = (char *)malloc(100 * sizeof(char));
            strcpy(currIpAdd.ipAdd, "127.0.0.1");
            currIpAdd.portNo = (char *)malloc(100 * sizeof(char));
            sprintf(currIpAdd.portNo, "%d", portNos[i]);
            bindUdp(&currIpAdd, &sFDs[i]);
        }
    }
    struct sockaddr_in serverAddress;

    int addressLength = sizeof(serverAddress);
    int count = 1;
    struct pollfd pFD[size];
    for (int i = 0; i < size; i++)
    {
        pFD[i].fd = sFDs[i];
        if (strcmp(allConf[i]->protName, "tcp") == 0)
            pFD[i].events = POLLIN;
        else
            pFD[i].events = POLLOUT;
    }

    while (1)
    {
        printf("At start of accept\n");
        fflush(stdout);
        int nFDs = size;
        int ready = poll(pFD, nFDs, -1);
        if (ready == -1)
            exit(0);
        for (int i = 0; i < size; i++)
        {
            if (pFD[i].fd == -1)
                continue;
            if (pFD[i].revents & (POLLIN | POLLOUT))
            {
                acceptHandler(pFD[i].fd, allConf[i]);
            }
        }
    }
    exit(0);
}
