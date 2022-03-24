#include <stdio.h>
#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
using namespace std;

int main(int argc, char *argv[]){
    int stdin = argv[1][0];
    int stdout = argv[1][1];

    char buff[100];
    read(0, buff, 100);
    write(stdout, buff, strlen(buff));
    printf("Hello from p2!\n"); // fd = 1
}



