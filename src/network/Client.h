#pragma once

#include <queue>
#include "Socket.h"
#include "Message.h"


class Client
{
    Socket client;
    sockaddr_in serverAddress;
    std::queue<Message> receivedPackets;

public:

    Client():
    client()
    {
        memset(&serverAddress, 0, sizeof(sockaddr_in));
    }

    bool open();
    void setServerAddress(const sockaddr_in& addr);
    bool isOpen(){return client.isOpen();}
    void getData();
    void sendData( const void* data, int len);
    void shutdown();


    unsigned storedPacketCount(){return receivedPackets.size();}
    Message* fetchOldestPacket();
    void discardOldestPacket();

};
