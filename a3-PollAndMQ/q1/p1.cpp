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
    system("g++ p3.cpp -o p3");
    system("g++ p2.cpp -o p2");
    system("g++ p4.cpp -o p4");
    int key = ftok("myMQ", 65);
    int mqId = msgget(key, 0666 | IPC_CREAT);
    // registering pids
    char *rgstr[3][10];
    // for (int i = 0; i < 3; i++)
    // {
    //     msgrcv(mqId, rgstr[i], 100, REGISTER, 0);
    //     write(STDOUT_FILENO, rgstr[i], 10);
    // }
    msgrcv(mqId, rgstr[0], 100, REGISTER, 0);
    write(STDOUT_FILENO, rgstr[0], 10);
    msgrcv(mqId, rgstr[1], 100, REGISTER, 0);
    write(STDOUT_FILENO, rgstr[1], 10);
    msgrcv(mqId, rgstr[2], 100, REGISTER, 0);
    write(STDOUT_FILENO, rgstr[2], 10);

    msg_st msgs[3];
    // for (int i = 0; i < 3; i++)
    // {
    //     msgs[i].type = i + 2;
    //     string msg = "From P1 to P" + (char)(i + '2');
    //     strcpy(msgs[i].msg_text, msg.c_str());
    //     msgsnd(mqId, &msgs[i], strlen(msgs[i].msg_text), msgs[i].type);
    // }

    msgs[0].type = 2;
    strcpy(msgs[0].msg_text, "From P1 to P2\n");
    msgsnd(mqId, &msgs[0], strlen(msgs[0].msg_text), msgs[0].type);
    msgs[1].type = 3;
    strcpy(msgs[1].msg_text, "From P1 to P3\n");
    msgsnd(mqId, &msgs[1], strlen(msgs[1].msg_text), msgs[1].type);
    msgs[2].type = 4;
    strcpy(msgs[2].msg_text, "From P1 to P4\n");
    msgsnd(mqId, &msgs[2], strlen(msgs[2].msg_text), msgs[2].type);

    return 0;
}