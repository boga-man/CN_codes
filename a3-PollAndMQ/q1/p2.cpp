#include <stdio.h>
#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <unistd.h>
using namespace std;

#define REGISTER 1

struct msg_st
{
    long int type;
    char msg_text[128];
};

int main()
{
    int key = ftok("myMQ", 65);
    int mqId = msgget(key, 0666 | IPC_CREAT);
    
    msg_st msg;
    msg.type = 2;
    strcpy(msg.msg_text, to_string(getpid()).c_str());
    msgsnd(mqId, &msg, strlen(msg.msg_text), REGISTER);

    msgrcv(mqId, &msg, strlen(msg.msg_text), 2, 0);

    cout<<msg.msg_text<<endl;
    cout<<getpid();
    cout<<"deb\n";

    return 0;
}