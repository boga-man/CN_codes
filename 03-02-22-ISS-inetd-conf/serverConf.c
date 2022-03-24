#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
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

int size = 0;

service_conf **readConfigurations(const char *fileName)
{
    service_conf **allConfs = (service_conf **)calloc(100, sizeof(struct service_conf_t *));
    FILE *f = fopen(fileName, "r");
    char currLine[500];
    char *returnStr = fgets(currLine, 500, f);
    while (returnStr != NULL)
    {
        allConfs[size++] = getServiceConf(currLine);
        printConf(allConfs[size - 1]);
        printf("Done\n");
        returnStr = fgets(currLine, 500, f);
    }
    return allConfs;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Need two arguments\n");
        exit(EXIT_FAILURE);
    }
    service_conf **allConf = readConfigurations(argv[1]);
    for (int i = 0; i < size; i++)
    {
        printConf(allConf[i]);
    }
}