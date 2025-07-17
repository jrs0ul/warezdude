#ifndef _CCLIENT_H_
#define _CCLIENT_H_


/*Client class
 jrs0ul
 2007 03 23
*/

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
    //konstruktorius
    CClient(){
        isJoinedServer=false;
    }
    //prisijungia prie serverio
    bool connectServer(const char* ip, unsigned port);
    //atsijungia
    void disconnectServer();
    //siuncia duomenis
    void sendData(const char* data, int len);
    //gauna duomenis
    int getData(char* data);
    //ar prisijunges
    bool isJoined(){return isJoinedServer;}

};


#endif //_CLIENT_H_
