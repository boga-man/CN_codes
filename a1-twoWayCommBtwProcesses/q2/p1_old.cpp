#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
using namespace std;

int main()
{
    pid_t pid;
    int parentFD[2], childFD[2];
    pipe(parentFD);
    pipe(childFD);
    cout << "Enter return to end communication between processes\n-----------------------------------------------------\n";
    int in = dup(0);
    int out = dup(0);
    pid = fork();
    if (pid == 0)
    {
        // dup2(parentFD[0], 0);
        // dup2(childFD[1], 1);
        char buff[5];
        memset(buff, 0, 3);
        buff[0] = in;
        buff[1] = out;
        buff[2] = parentFD[0];
        buff[3] = childFD[1];
        close(parentFD[1]);
        close(childFD[0]);
        char *argv[] = {"./p2", buff, NULL};
        execv("./p2", argv);
    }
    else
    {
        close(parentFD[0]);
        close(childFD[1]);
        while (true)
        {
            cout << "enter the message to be written by parent process: "<< "\n";
            char buff[100];
            int inp = read(0, buff, 100);
            if (inp == 0 || (inp == 1 && buff[0] == '\n'))
                exit(EXIT_SUCCESS);
            write(parentFD[1], buff, inp);
            memset(buff, 0, 100);
            int readbytes = read(childFD[0], buff, 100);
            if (readbytes == 0)
                exit(EXIT_SUCCESS);
            cout << "message read by parent process: ";
            cout << buff<< endl;
        }
        
    }
}