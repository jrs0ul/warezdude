#ifndef _CSERVER_H_
#define _CSERVER_H_

//Server class


#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
#endif

#include <vector>
#include <queue>
#include "Socket.h"
#include "Message.h"


struct ClientFootprint
{
    sockaddr_in address;
    unsigned ID;
    long lastMsgTimestamp;
};


class Server
{

    Socket server;
    std::vector<ClientFootprint> connectedClientAddreses;

    std::queue<Message> receivedPackets;

public:


    unsigned clientCount();

    bool isRunning();

    void shutDown();

    bool launch(int port);

    void getData();

    void sendData(unsigned clientIndex, const char* data, int len);

    void addClient(const ClientFootprint& fp);
    void removeClient(unsigned index);

    unsigned storedPacketCount(){return receivedPackets.size();}
    Message* fetchOldestPacket();
    void discardOldestPacket();

    int findClientByAddress(const sockaddr_in& addr);
};




#endif //_CSERVER_H_
