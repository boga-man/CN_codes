#include "myHeader.h"

int udpsfd, PACK_PORT;
string ip, port;

void *sender(void *arg){
    // store the package udp server address
    struct sockaddr_in paddr;
    paddr.sin_family = AF_INET;
    paddr.sin_port = htons(PACK_PORT);
    if (inet_pton(AF_INET, ip.c_str(), &paddr.sin_addr) <= 0)
    {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }
    // now send the sms to other clients
    while(1){
        cout<<"Enter the message to clients\n";
        cout<<"Format: CLIENTNO:<MESSAGE>\n";
        string msg;
        cin>>msg;
        int n = sendto(udpsfd, msg.c_str(), msg.size(), 0, (struct sockaddr *)&paddr, sizeof(paddr));
        if(n<=0){
            perror("sendto");
            continue;
        }
        // char buff[1024] = {0};
        // n = recvfrom(udpsfd, buff, 1024, 0, NULL, NULL);
        // cout<<buff<<endl;
    }
}

void *receiver(void *arg){
    while(1){
        // keep reading on udpsfd
        char buff[1024] = {0};
        int n = recvfrom(udpsfd, buff, 1024, 0, NULL, NULL);
        if (n <= 0)
        {
            perror("recvfrom");
            continue;
        }

        cout<<"A message received! => "<<buff<<endl;
    }
}

int main(int argc, char *argv[])
{
    // first the client connects to the NC1
    int sfd = ConnectToTcpServer(sysIp, NC1_CLI_PORT);

    // create a udp client endpoint
    if ((udpsfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    

    // get the address of the udp endpoint
    struct sockaddr_in myaddr;
    int len = sizeof(myaddr);
    getsockname(udpsfd, (struct sockaddr *)&myaddr, (socklen_t *)&len);

    // make a string out of the address
    char myIp[16];
    inet_ntop(AF_INET, &myaddr.sin_addr, myIp, sizeof(myIp));
    int myPort = ntohs(myaddr.sin_port);
    string ip(myIp);
    ip = ip + ";" + to_string(myPort);
    cout<<"Sending IP and port to NC1: "<<ip<<"\n";

    // send this address to nc
    send(sfd, ip.c_str(), ip.size(), 0);

    // then send the required service and package
    string s;
    cout << "What service do you want?\nfreesms/paidsms/live\n";
    cin >> s;
    // send this service information to sfd
    WriteFD(sfd, s);

    // then client has to get the address of the package
    string packAddr = ReadFD(sfd);
    cout<<"Received package address: "<<packAddr<<endl;
    // get the ip address and port of package from the string
    getIpAndPort(ip, port, packAddr);

    // after getting the ip and port, client has to send udp msgs to package to avail services
    PACK_PORT = atoi(port.c_str());

    // while(1);

    // sender thread to send messages
    pthread_t stid;
    pthread_create(&stid, NULL, sender, NULL);

    // receiver thread to receive messages
    pthread_t rtid;
    pthread_create(&rtid, NULL, receiver, NULL);

    pthread_join(stid, NULL);
    pthread_join(rtid, NULL);
    
}