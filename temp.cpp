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
#include <sys/un.h>
using namespace std;

int cnt  = 0;

void handler(int sig){
    if(cnt == 5){
        SIG_DFL(SIGINT);
    }else{
        cnt++;
    }
}

int main(){
    signal(SIGINT, handler);

    while(1);
}