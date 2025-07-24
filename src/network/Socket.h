#pragma once


#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
#endif


struct Address
{
    uint32_t address;
    int port;
};


class Socket
{

#ifdef _WIN32
    WSADATA wsaData;
    SOCKET sock;
#else
    int sock;
#endif

    bool _open;

public:

    Socket(){_open = false;}

    bool openAsServer(int port);
    bool openAsClient();

    bool isOpen(){return _open;}

    int getData(void *data, size_t dataSize, Address& sender);

    void sendData( const void* data, int len, Address& destination);

    void shutdown();

};
