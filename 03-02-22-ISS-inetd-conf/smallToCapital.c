#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
int main()
{
    while (1)
    {
        char *buff = (char *)calloc(200, sizeof(char));

        scanf("%s", buff);
        if (strcmp(buff, "exit") == 0)
            break;
        int len = strlen(buff);
        for (int i = 0; i < len; i++)
        {
            if (buff[i] >= 'a' && buff[i] <= 'z')
            {
                buff[i] = buff[i] - 'a' + 'A';
            }
        }
        printf("%s", buff);
        fflush(stdout);
        free(buff);
    }
    return 0;
}