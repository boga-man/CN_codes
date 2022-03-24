#include <stdio.h>
#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
using namespace std;

void *readthread(void *arg)
{	
	while(1)
	{
		string s="";
		cout<<"Enter message in server: "<<endl;
		getline(cin,s);
		if(s.length()>0)
		{
			int pd=open("server",O_RDONLY);
			char * buffer=&s[0];
			write(pd,buffer,s.length());
			close(pd);
		}
	}
}


void *writethread(void *arg)
{
	while(1)
	{
		int fd = open("server", O_RDONLY);
		char ch[1024];
		read(fd, ch, 1024); 
		close(fd);
        string s(ch);
        if(s.length())
        {
        	string str = "Via server: "+s;
    	    int pd1=open("client1",O_WRONLY);
			char * buffer1=&str[0];
			write(pd1,buffer1,str.length());
			close (pd1);
			int pd2=open("client2",O_WRONLY);
			write(pd2,buffer1,str.length());
			close (pd2);
			int pd3=open("client3",O_WRONLY);
			write(pd3,buffer1,str.length());
			close (pd3);
		}
		
	}
}
int main()
{
	pthread_t t1,t2;
	mkfifo("server",0666);
	mkfifo("client1",0666);
	mkfifo("client2",0666);
	mkfifo("client3",0666);
    cout<<"Chat server is listening....\n";
	pthread_create(&t1,NULL,readthread,NULL);
	pthread_create(&t2,NULL,writethread,NULL);
	pthread_join(t1,NULL);
	pthread_join(t2,NULL);
}