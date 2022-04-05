#include "myHeader.h"

vector<int> pack_sfds;
vector<string> sms;
vector<string> live;

int sfd;

void *acceptPackages(void *arg)
{
    // using a tcp connection to connect to the packages
    sfd = TcpServerInit(NC1_PACK_PORT);

    // listen on sfd
    listen(sfd, 10);

    // accept the packages continuously
    while (1)
    {
        // accept the packages
        int nsfd = accept(sfd, NULL, NULL);
        // add the nsfd to the vector
        pack_sfds.push_back(nsfd);
        // storing the package information : IP and port
        char buff[1024] = {0};
        recv(nsfd, buff, sizeof(buff), 0);
        string serv(buff);
        // if there is prefix called sms in buff, then add the remaining suffix to sms
        // otherwise if there is a prefix called live, then add the remaining suffix to live
        if (serv.find("sms") != string::npos)
        {
            sms.push_back(serv.substr(4));
            string ip, port;
            getIpAndPort(ip, port, serv);
            p_sms_udp_ports.push_back(atoi(port.c_str()));
            cout << "Added an sms service\n";
        }
        else if (serv.find("live") != string::npos)
        {
            live.push_back(serv.substr(5));
            string ip, port;
            getIpAndPort(ip, port, serv);
            p_live_udp_ports.push_back(atoi(port.c_str()));
            cout << "Added a live service\n";
        }
    }
}

void sigHand(int sig)
{
    system("rm usfd");
    SIG_DFL(SIGINT);
    return;
}

void sendCliIp(string ipAddr)
{
    // int i = 0;
    // int udpsfd;
    // if ((udpsfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    // {
    //     perror("socket creation failed");
    //     exit(EXIT_FAILURE);
    // }

    // for (string x : sms)
    // {
    //     // sending the client ip and port to all sms packages
    //     string ip, port;
    //     getIpAndPort(ip, port, x);
    //     struct sockaddr_in caddr;
    //     if (inet_pton(AF_INET, ip.c_str(), &caddr.sin_addr) <= 0)
    //     {
    //         perror("inet_pton");
    //         exit(EXIT_FAILURE);
    //     }
    //     caddr.sin_family = AF_INET;
    //     caddr.sin_port = htons(p_sms_udp_ports[i++]);

    //     // send ipAddr to sms package
    //     sendto(udpsfd, (const char *)ipAddr.c_str(), strlen(ipAddr.c_str()),
    //            MSG_CONFIRM, (const struct sockaddr *)&caddr,
    //            sizeof(caddr));
    // }
    // i = 0;
    // for (string x : live)
    // {
    //     // sending the client ip and port to all sms packages
    //     string ip, port;
    //     getIpAndPort(ip, port, x);
    //     struct sockaddr_in caddr;
    //     if (inet_pton(AF_INET, ip.c_str(), &caddr.sin_addr) <= 0)
    //     {
    //         perror("inet_pton");
    //         exit(EXIT_FAILURE);
    //     }
    //     caddr.sin_family = AF_INET;
    //     caddr.sin_port = htons(p_live_udp_ports[i++]);

    //     // send ipAddr to sms package
    //     sendto(udpsfd, (const char *)ipAddr.c_str(), strlen(ipAddr.c_str()),
    //            MSG_CONFIRM, (const struct sockaddr *)&caddr,
    //            sizeof(caddr));
    // }

    for(int x: pack_sfds){
        send(x, ipAddr.c_str(), ipAddr.length(), 0);
    }
}

int main()
{
    // remove the uds socket file on stopping the process
    signal(SIGINT, sigHand);
    // first connect with nc2 via uds
    int usfd = createListenUsfd(usfd_path);

    // accept the connection from nc2
    int nusfd = accept(usfd, NULL, NULL);
    // ------------- UDS is working -------------

    // connecting to the services available in a separate thread
    pthread_t packtid;
    pthread_create(&packtid, NULL, acceptPackages, NULL);

    // accept the clients in main thread itself
    // open a well known tcp sfd with NC1_CLI_PORT
    int sfd = TcpServerInit(NC1_CLI_PORT);

    // listen on sfd
    listen(sfd, 10);
    cout << "Listening for clients";

    // accept the clients continuously and check if requested service is available or not
    while (1)
    {
        struct sockaddr_in caddr;
        memset(&caddr, 0, sizeof(caddr));
        int len = sizeof(caddr);
        int nsfd = accept(sfd, (struct sockaddr *)&caddr, (socklen_t *)&len);
        // after a client connects, send it's address to all the packages
        // get the ip address from caddr
        // char myIp[16];
        // inet_ntop(AF_INET, &caddr.sin_addr, myIp, sizeof(myIp));
        // int myPort = ntohs(caddr.sin_port);
        // string ip(myIp);
        // ip = ip + ";" + to_string(myPort);
        char myIp[1024] = {0};
        int n = recv(nsfd, myIp, sizeof(myIp), 0);
        string ip(myIp);
        // send to all the packages
        sendCliIp(ip);


        // get the package information from client and print it
        char buff[1024] = {0};
        n = recv(nsfd, buff, 1024, 0);

        // handling possible errors
        if (n == 0)
        {
            cout << "Client disconnected\n";
            close(nsfd);
            continue;
        }
        if (n <= -1)
        {
            perror("accept");
            kill(getpid(), SIGINT);
        }

        string serv(buff);
        // search if the service is available in the server or not
        if (serv.find("freesms") != string::npos || serv.find("paidsms") != string::npos)
        {
            if (sms.size() == 0)
            {
                cout << "No sms services available\nSending this client to NC2\n";
                // send the client to NC2
                send_fd(nusfd, nsfd);
                close(nsfd);
                continue;
            }
            // if the service is available, send the package address to the client
            else
            {
                cout << "Sending sms service information to client\n";
                // send the package address to the client
                int i = rand() % sms.size();
                cout<<sms[i]<<endl;
                int n = WriteFD(nsfd, sms[i]);
                continue;
            }
        }
        else if (serv.find("live") != string::npos)
        {
            if (live.size() == 0)
            {
                cout << "No live services available\nSending this client to NC2\n";
                // send the client to NC2
                send_fd(nusfd, nsfd);
                close(nsfd);
                continue;
            }
            // if the service is available, send the package address to the client
            else
            {
                cout << "Sending live service information to client\n";
                // send the package address to the client
                int i = rand() % sms.size();
                int n = WriteFD(nsfd, live[i]);
                continue;
            }
        }
    }
}