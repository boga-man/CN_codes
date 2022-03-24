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
    mkfifo("/tmp/fifo2", 0666);
    int fd = open("/tmp/fifo2", O_WRONLY);
    write(fd, "From p4\n", strlen("from p3\n"));
    close(fd);
    return 0;
}