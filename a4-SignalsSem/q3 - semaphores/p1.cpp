#include <stdio.h>
#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
using namespace std;

const char *s12 = "/s12";
const char *s41 = "/s41";

int main(){
    system("g++ p2.cpp -o p2 -lpthread");
    system("g++ p3.cpp -o p3 -lpthread");
    system("g++ p4.cpp -o p4 -lpthread");
    sem_t *S12 = sem_open(s12, O_CREAT|O_EXCL, 0666, 0);
    sem_t *S41 = sem_open(s41, O_CREAT|O_EXCL, 0666, 0);
    cout<<"I am P1, enter any character to sem-signal(S12)\n";
    char c; cin>>c;
    cout<<"I am signalling semaphore signal of S12\n";
    S12 = sem_open(s12, O_EXCL);
    sem_post(S12);
    cout<<"I am waiting for semaphore S41\n";
    S41 = sem_open(s41, O_EXCL);
    sem_wait(S41);
    cout<<"I got semaphore signalling from P4\n";

    sem_unlink(s12);
    sem_unlink(s41);
    return 0;
}