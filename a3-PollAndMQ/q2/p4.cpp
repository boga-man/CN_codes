#include <stdio.h>
#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <poll.h>
#include <unistd.h>
using namespace std;

int main(){
    mkfifo("/tmp/fifo3", 0666);
    int fd = open("/tmp/fifo3", O_WRONLY);
    write(fd, "From p4\n", strlen("from p4\n"));
    close(fd);
    return 0;
}