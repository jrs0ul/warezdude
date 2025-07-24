#include "CServer.h"
#include <cstdio>
#include <arpa/inet.h>



//--------------------------------------------------------------
unsigned Server::clientCount()
{
    return connectedClientAddreses.count();
}

//---------------------------------------------------------------
void Server::shutDown()
{
    server.shutdown();
}
//-------------------------------------------------------------------------
bool Server::launch(int port)
{
    return server.openAsServer(port);
}
//----------------------------------------------
int Server::getData(void *data, size_t dataSize, sockaddr_in& senderAddress)
{
    return server.getData(data, dataSize, senderAddress);
}

//----------------------------------------------
void Server::sendData(unsigned clientIndex, const char *data, int len)
{

    if (clientIndex >= connectedClientAddreses.count())
    {
        printf("send fail\n");
        return;
    }

    sockaddr_in a;
    a.sin_family      = connectedClientAddreses[clientIndex].address.sin_family;
    a.sin_port        = connectedClientAddreses[clientIndex].address.sin_port;
    a.sin_addr.s_addr = connectedClientAddreses[clientIndex].address.sin_addr.s_addr;

    server.sendData(data, len, a);
}
//-------------------------------------------
void Server::removeClient(unsigned index)
{

    connectedClientAddreses.remove(index);

}

//-------------------------------------
bool Server::isRunning()
{
    return server.isOpen();
}
//-------------------------------------
void Server::addClient(const ClientFootprint& fp)
{
    connectedClientAddreses.add(fp);
}
