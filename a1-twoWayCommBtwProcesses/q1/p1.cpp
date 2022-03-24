#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
using namespace std;

int main()
{
    int parent_child_pipe[2];
    int child_parent_pipe[2];
    pipe(parent_child_pipe);
    pipe(child_parent_pipe);
    printf("Beginning communication between parent and child\n");
    printf("Press return without any input to end communication\n-------------------------------------------------\n");
    int pid = fork();
    if (pid == 0)
    {
        close(parent_child_pipe[0]);
        close(child_parent_pipe[1]);
        char message[100];
        while (strcmp(message, "") != 0)
        {
            printf("Enter the message to the parent:\n");
            cin.getline(message, 100);
            write(parent_child_pipe[1], message, sizeof(message));
            if (strcmp(message, "") == 0)
            {
                break;
            }
            read(child_parent_pipe[0], message, sizeof(message));
            if (strcmp(message, "") == 0)
            {
                break;
            }
            printf("Message from the parent: %s\n\n", message);
        }
        printf("NULL input detected. Exiting Child\n");
    }
    else
    {
        close(parent_child_pipe[1]);
        close(child_parent_pipe[0]);
        char message[100];
        while (strcmp(message, "") != 0)
        {
            read(parent_child_pipe[0], message, sizeof(message));
            if (strcmp(message, "") == 0)
            {
                break;
            }
            printf("Message from the child: %s\n\n", message);
            printf("Enter the message to the child:\n");
            cin.getline(message, 100);
            write(child_parent_pipe[1], message, sizeof(message));
            if (strcmp(message, "") == 0)
            {
                break;
            }
        }
        printf("NULL input detected. Exiting Parent\n");
    }
}
