#include "Server.h"
#include <cstdio>
#ifndef _WIN32
#include <arpa/inet.h>
#endif


//--------------------------------------------------------------
unsigned Server::clientCount()
{
    return connectedClientAddreses.size();
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
void Server::getData()
{
    Message msg;
    memset(&msg.senderAddress, 0, sizeof(sockaddr_in));

    int len = server.getData(msg.data, MAX_MESSAGE_DATA_SIZE, msg.senderAddress);

    if (len)
    {

        msg.length = len;
        msg.parsed = false;
        receivedPackets.push(msg);

    }
}

//----------------------------------------------
void Server::sendData(unsigned clientIndex, const char *data, int len)
{

    if (clientIndex >= connectedClientAddreses.size())
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

    connectedClientAddreses.erase(connectedClientAddreses.begin() + index);

}

//-------------------------------------
bool Server::isRunning()
{
    return server.isOpen();
}
//-------------------------------------
void Server::addClient(const ClientFootprint& fp)
{
    connectedClientAddreses.push_back(fp);
}
//----------------------------------
Message* Server::fetchOldestPacket()
{
    return &receivedPackets.front();
}
//-----------------------------------
void Server::discardOldestPacket()
{
    receivedPackets.pop();
}
//-----------------------------------
int Server::findClientByAddress(const sockaddr_in& addr)
{

    for (unsigned i = 0; i < connectedClientAddreses.size(); ++i)
    {
        if ((connectedClientAddreses[i].address.sin_port == addr.sin_port) &&
            (connectedClientAddreses[i].address.sin_addr.s_addr == addr.sin_addr.s_addr))
        {
            return i;
        }
    }

    return -1;
}
