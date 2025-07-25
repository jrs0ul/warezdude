#ifndef _CSERVER_H_
#define _CSERVER_H_

//Server class


#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
#endif

#include "../DArray.h"
#include "Socket.h"



struct ClientFootprint
{
    sockaddr_in address;
    unsigned ID;
    long lastMsgTimestamp;
};


class Server
{

    Socket server;
    DArray<ClientFootprint> connectedClientAddreses;

public:


    unsigned clientCount();

    bool isRunning();

    void shutDown();

    bool launch(int port);

    int getData(void *data, size_t dataSize, sockaddr_in& senderAddress);

    void sendData(unsigned clientIndex, const char* data, int len);

    void addClient(const ClientFootprint& fp);
    void removeClient(unsigned index);

    int findClientByAddress(const sockaddr_in& addr);
};




#endif //_CSERVER_H_
