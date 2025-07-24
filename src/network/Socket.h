#pragma once


#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
#endif



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

    int getData(void *data, size_t dataSize, sockaddr_in& sender);

    void sendData( const void* data, int len, sockaddr_in& destination);

    void shutdown();

};
