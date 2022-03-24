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

int main(int argc, char **argv)
{
    int std_in = argv[1][0];
    int std_out = argv[1][1];
    // int readFD = 0, writeFD = 1;
    int readFD = argv[1][2];
    int writeFD = argv[1][3];
    while (true)
    {
        char buff[100];
        memset(buff, 0, 100);
        int readbytes = read(readFD, buff, 100);
        if (readbytes == 0)
            exit(0);
        else
        {
            string message = "Message received by grand child via child process: ";
            message += string(buff);
            char mess[message.length()];
            strcpy(mess, message.c_str());
            write(std_out, mess, strlen(mess));
        }
        string message = "Enter the message to be sent to parent by the grand child: ";
        char mess[message.length()];
        strcpy(mess, message.c_str());
        write(std_out, mess, strlen(mess));
        buff[100];
        int inp = read(std_in, buff, 100);
        if (inp == 0 || (inp == 1 && buff[0] == '\n'))
            exit(0);
        write(writeFD, buff, inp);
    }
}