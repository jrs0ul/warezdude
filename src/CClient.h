#ifndef _CCLIENT_H_
#define _CCLIENT_H_


//Client class

#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
#endif

class CClient{
#ifdef _WIN32
    SOCKET clientSock;
    SOCKADDR_IN _sin;
#else
    int clientSock;
    sockaddr_in _sin;

#endif
    bool isJoinedServer;

public:
    CClient(){
        isJoinedServer=false;
    }
    //connect to the server with such ip and port
    bool connectServer(const char* ip, unsigned port);
    //disconnect the server
    void disconnectServer();
    //sends bytes to the server
    void sendData(const char* data, int len);
    //receives bytes
    int getData(char* data);
    //is the client conneted to the server
    bool isJoined(){return isJoinedServer;}

};


#endif //_CLIENT_H_
