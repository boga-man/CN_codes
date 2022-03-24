#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>

int nfds = 3;

int main()
{
    system("g++ ./p2.cpp -o p2");
    system("g++ ./p3.cpp -o p3");
    system("g++ ./p4.cpp -o p4");
    // creating the fifos
    mkfifo("/tmp/fifo1", 0666);
    mkfifo("/tmp/fifo2", 0666);
    mkfifo("/tmp/fifo3", 0666);

    struct pollfd pfd[nfds];
    printf("dev\n");
    pfd[0].fd = open("/tmp/fifo1", O_RDONLY);
    pfd[0].events = POLLIN;
    pfd[0].revents = 0;
    printf("Opened first fifo\n");
    pfd[1].fd = open("/tmp/fifo2", O_RDONLY);
    pfd[1].events = POLLIN;
    pfd[1].revents = 0;
    printf("Opened second fifo\n");
    pfd[2].fd = open("/tmp/fifo3", O_RDONLY);
    pfd[2].events = POLLIN;
    pfd[2].revents = 0;
    printf("Opened third fifo\n");

    while (1)
    {
        printf("Waiting for some event to occur\n");
        int status = poll(pfd, nfds, -1);
        // cout << "Poll status: " << status << endl;
        if (status == 0)
        {
            printf("No input is ready with data\nRetrying...\n");
            continue;
        }
        else if (status == -1)
        {
            printf("Error while polling\nExiting\n");
            perror("Poll");
            exit(1);
        }
        for (int i = 0; i < nfds; i++)
        {
            char buff[128];
            if (pfd[i].revents & POLLIN)
            {
                int c = read(pfd[i].fd, buff, 128);
                write(1, buff, c);
                pfd[i].revents = 0;
            }
        }
    }

    for (int i = 0; i < nfds; i++)
    {
        close(pfd[i].fd);
    }
    return 0;
}