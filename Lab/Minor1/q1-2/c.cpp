#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#define MAX 80
#define PORT 8080
#define SA struct sockaddr

int main()
{
	int sfd, connfd;
	struct sockaddr_in servaddr, cli;

	// socket create and verification
	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sfd == -1) {
		perror("socket");
		exit(0);
    }
	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);

	// connect the client socket to server socket
	if (connect(sfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		perror("connect");
		exit(0);
	}

	char buff[MAX];
	int n;
    bzero(buff, sizeof(buff));
    printf("Enter the string : ");
    n = 0;
    while ((buff[n++] = getchar()) != '\n')
        ;
    write(sfd, buff, sizeof(buff));
    bzero(buff, sizeof(buff));
    read(sfd, buff, sizeof(buff));
    printf("From Server : %s", buff);

	// close the socket
	close(sfd);

    return 0;
}