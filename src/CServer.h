#ifndef _CSERVER_H_
#define _CSERVER_H_

/*Server class
 jrs0ul
 2007 03 17
*/


#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
#endif

#include "DArray.h"

class CServer{

#ifdef _WIN32
    WSADATA wsaData;
    DArray<SOCKET> clientSocks; //klientu socketai
    DArray<sockaddr_in> clientAddreses; // klientu adresai
    SOCKET serverSock;    //serverio socketas
#else
    int serverSock;
    DArray<int> clientSocks;
    DArray<sockaddr_in> clientAddreses; // klientu adresai

#endif
    unsigned _clientCount; //kiek atviru kliento socketu
    bool isServerRunning; //ar serveris veikia
    
public:
    //konstruktorius
    CServer(){  _clientCount=0; isServerRunning=false;}
    //grazina prisijungusiu klientu skaiciu
    unsigned clientCount(){return _clientCount;}
    //grazina serverio busena online/offline
    bool serverState(){return isServerRunning;}
    //isjungia serveri
    void shutDown();
    //ijungia serveri
    bool launch(int port);
    //laukia kliento ir leidzia jam prisijungti
    int waitForClient();
    //gauna duomenis
    int getData(int clientIndex, char* buf);
    //siuncia duomenis
    void sendData(unsigned clientIndex, const char* data, int len);
    //panaikina norima klienta
    void removeClient(unsigned index);
};




#endif //_CSERVER_H_
