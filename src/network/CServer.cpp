#include "CServer.h"
#include <cstdio>
#ifndef _WIN32

#endif



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
int Server::getData(void *data, size_t dataSize, Address& senderAddress)
{
    return server.getData(data, dataSize, senderAddress);
}

//----------------------------------------------
void Server::sendData(unsigned clientIndex, const char *data, int len)
{

    if (clientIndex >= connectedClientAddreses.count())
    {
        return;
    }

    server.sendData(data, len, connectedClientAddreses[clientIndex].address);
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
