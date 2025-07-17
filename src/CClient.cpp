#include "CClient.h"

#ifndef _WIN32
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/types.h>
#endif
/*Client class
 jrs0ul
 2007 03 23
*/

bool CClient::connectServer(const char* ip, unsigned port)
{
#ifdef _WIN32
    WSAData wsaData;
    WSAStartup(MAKEWORD(2,0),&wsaData);
#endif
    clientSock = socket(AF_INET,SOCK_STREAM,0);

#ifdef _WIN32
    if (clientSock == INVALID_SOCKET)
#else
    if (clientSock == -1)
#endif
    {
        return false;
    }

#ifdef _WIN32
    LPHOSTENT hostEntry;
#else
    hostent* hostEntry;
#endif
    hostEntry = gethostbyname(ip);

    if (!hostEntry)
    {
        return false;
    }

    _sin.sin_family = AF_INET;
#ifdef _WIN32
    _sin.sin_addr = *((LPIN_ADDR)*hostEntry->h_addr_list);
#else
    _sin.sin_addr = *((in_addr*)*hostEntry->h_addr_list);
#endif
    _sin.sin_port = htons(port);

#ifdef _WIN32
    if (connect(clientSock,(LPSOCKADDR)&_sin, sizeof(sockaddr_in))!= SOCKET_ERROR)
#else
    if (connect(clientSock,(sockaddr*)&_sin, sizeof(sockaddr_in))!= -1)
#endif
    {
        isJoinedServer=true;
        return true;
    }

    return false;
}
//-----------------------------------


void CClient::disconnectServer()
{
    if (isJoinedServer)
    {
#ifdef _WIN32
        closesocket(clientSock);
        WSACleanup();
#else
        close(clientSock);
#endif
        isJoinedServer=false;
    }
}

//-----------------------------------

void CClient::sendData(const char *data, int len)
{
    send(clientSock,data,len,0);
}

//-----------------------------------

int CClient::getData(char* data)
{

#ifdef _WIN32
    FD_SET set;
#else
    fd_set set;
#endif

    timeval   waitTimeStr;

    FD_ZERO( &set );
    FD_SET(clientSock, &set );
    waitTimeStr.tv_sec = 0;  
    waitTimeStr.tv_usec = 0;

    int res=select(FD_SETSIZE,&set,0,0, &waitTimeStr);
    if (res>0){
        int bytes=recv(clientSock,data,1024,0);
        return bytes;
    }
    else
        return 0;

}
