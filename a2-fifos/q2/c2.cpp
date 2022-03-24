#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include<iostream>
using namespace std;

void * writethread(void *arg)
{
	while(1)
	{
		int fd=open("server",O_WRONLY);
		string s="";
		cout<<"\nYour message: ";
		getline(cin,s);
		s="C2:"+s;
		char *buff=&s[0];
		write(fd,buff,s.length());
		close(fd);
	}
}
void * readthread(void *arg)
{
	while(1)
	{
	//	cout<<"hi"<<endl;
		int pd=open("client2",O_RDONLY);
		char ch[1024]={'\0'};
		read(pd, ch, 1024); 
		string s(ch);
		if(s.length())
		cout<<s<<endl;
		close(pd);
	}
}
int main()
{
	pthread_t t1,t2;
	mkfifo("server",0666);
	mkfifo("client2",0666);
	pthread_create(&t1,NULL,readthread,NULL);
	pthread_create(&t2,NULL,writethread,NULL);
	pthread_join(t1,NULL);
	pthread_join(t2,NULL);
	
	
}