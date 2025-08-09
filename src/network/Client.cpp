#include "Client.h"


bool Client::open()
{
    return client.openAsClient();
}

void Client::setServerAddress(const sockaddr_in& addr)
{
    serverAddress = addr;
}

void Client::getData()
{
    Message msg;
    memset(&msg.senderAddress, 0, sizeof(sockaddr_in));

    int len = client.getData(msg.data, MAX_MESSAGE_DATA_SIZE, msg.senderAddress);


    if ((msg.senderAddress.sin_port != serverAddress.sin_port) ||
            (msg.senderAddress.sin_addr.s_addr != serverAddress.sin_addr.s_addr))
    {
        return;
    }

    if (len)
    {

        msg.length = len;
        msg.parsed = false;
        receivedPackets.push(msg);

    }

}


void Client::sendData( const void* data, int len)
{
    client.sendData(data, len, serverAddress);
}

void Client::shutdown()
{
    std::queue<Message> empty;
    receivedPackets.swap(empty);
    client.shutdown();
}


Message* Client::fetchOldestPacket()
{
    return &receivedPackets.front();
}

void Client::discardOldestPacket()
{
    receivedPackets.pop();
}
