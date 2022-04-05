#include "myHeader.h"

int udpsfd, sfd;
vector<string> clients;

void getCliNoAndMsg(string &cliNo, string &msg, string info)
{
    int i = 0;

    while (msg[i] != ':')
    {
        cliNo += msg[i++];
    }
    i++;
    while (msg[i] != '\0')
    {
        msg += msg[i++];
    }
}

void *storeCli(void *arg)
{
    int PORT = *(int *)arg;
    // have to receive udp msgs from nc and store the clients addresses
    // create udp server
    

    while (1)
    {
        char buff[1024] = {0};
        // int n = recvfrom(udpsfd, (char *)buff, 1024, MSG_WAITALL, NULL, NULL);
        int n = recv(sfd, buff, sizeof(buff), 0);
        if (n <= 0)
        {
            perror("recv");
            continue;
        }

        // store the client address information
        string ip, port;
        string info(buff);
        getIpAndPort(ip, port, info);
        ip = ip + ";" + port;
        clients.push_back(ip);
        cout<<"A client "<<ip<<" entered\n";
    }
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        cout << "Usage: ./nc NC_NUM SERV_TYPE PORT_OF_THIS_CLIENT" << endl;
        return 0;
    }

    int PORT = atoi(argv[3]);
    cout << "My tcp port will be: " << PORT << endl;

    int NC_PACK_PORT;
    if (argv[1][0] == '1')
    {
        NC_PACK_PORT = NC1_PACK_PORT;
    }
    else
    {
        NC_PACK_PORT = NC2_PACK_PORT;
    }
    // first connect with nc via tcp
    sfd = ConnectToTcpServer("127.0.0.1", NC_PACK_PORT);
    // after connecting, send its service to nc
    cout << "My service is " << argv[2] << endl;
    string s(argv[2]);
    string ip = "0.0.0.0";
    string port(argv[3]);
    s = s + "@" + ip + ";" + port;
    WriteFD(sfd, s);
    cout << "Connected to NC server\n";
    // ----------------- package connected to the NC ------------------

    udpsfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpsfd < 0)
    {
        perror("socket");
        exit(1);
    }

    // Filling server information
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // Bind the socket with the server address
    if (bind(udpsfd, (const struct sockaddr *)&servaddr,
             sizeof(servaddr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // create a thread that can store the clients addresses
    pthread_t ctid;
    pthread_create(&ctid, NULL, storeCli, &PORT);

    // while(1);

    // now forward the messages requests to other clients
    char buff[1024] = {0};
    while (1)
    {
        // recv a message from a client
        int n = recvfrom(udpsfd, buff, 1024, 0, NULL, NULL);
        if (n <= 0)
        {
            perror("recvfrom");
            continue;
        }

        // decode the message: clientnum and message
        string info(buff);
        string cliNo, msg;
        getCliNoAndMsg(cliNo, msg, info);

        // forward the msg to cliNo address
        int cliNo_int = atoi(cliNo.c_str());
        struct sockaddr_in caddr;
        caddr.sin_family = AF_INET;
        string caddr_str = clients[cliNo_int];
        string ip, port;
        getIpAndPort(ip, port, caddr_str);
        caddr.sin_port = htons(atoi(port.c_str()));
        if (inet_pton(AF_INET, ip.c_str(), &caddr.sin_addr) <= 0)
        {
            perror("inet_pton");
            exit(EXIT_FAILURE);
        }
        sendto(udpsfd, msg.c_str(), msg.size(), 0, (struct sockaddr *)&caddr, sizeof(caddr));
    }
}