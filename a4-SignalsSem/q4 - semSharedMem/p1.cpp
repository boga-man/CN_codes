#include <stdio.h>
#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
using namespace std;

key_t shmkeyx = ftok("./shmx_file", 65);
key_t shmkeyy = ftok("./shmy_file", 65);

const char *semx = "s1";
const char *semy = "s2";

int main(){
    int shmidx = shmget(shmkeyx, 16, 0666|IPC_CREAT);
    int shmidy = shmget(shmkeyy, 16, 0666|IPC_CREAT);

    int *x = (int *)shmat(shmidx, 0, 0);
    int *y = (int *)shmat(shmidy, 0, 0);

    sem_t *S1 = sem_open(semx, O_CREAT|O_EXCL, 0666, 0);
    sem_t *S2 = sem_open(semy, O_CREAT|O_EXCL, 0666, 0);

    S1 = sem_open(semx, O_EXCL);
    S2 = sem_open(semy, O_EXCL);
    
    *x = 1;*y = 1;

    cout<<"I am reading shm y\n";
    int read_y = *y;
    *x = read_y + 1;
    
    cout<<"Enter any character to signal S1\n";
    
    char c; cin>>c;
    sem_post(S1);
    
    cout<<"Waiting for S2\n";
    
    sem_wait(S2);

    cout<<"x = "<<*x<<" and y = "<<*y<<endl;
    shmdt(x);
    shmdt(y);

    shmctl(shmidx,IPC_RMID,NULL);
    shmctl(shmidy,IPC_RMID,NULL);
    
    sem_unlink(semx);
    sem_unlink(semy);
    return 0;
}