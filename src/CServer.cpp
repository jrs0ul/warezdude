#include "CServer.h"
#include <cstdio>
#ifndef _WIN32
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <sys/select.h>
#include <fcntl.h>
#include <sys/types.h>
#endif

//---------------------------------------------------------------
void CServer::shutDown(){

    if (isServerRunning)
    {

#ifdef _WIN32
        closesocket(serverSock);
#else
        close(serverSock);
#endif
        if (_clientCount){
            for (unsigned int i=0;i<_clientCount;i++)
            {
#ifdef _WIN32
                closesocket(clientSocks[i]);
#else
                close(clientSocks[i]);
#endif
            }

            clientSocks.destroy();
            clientAddreses.destroy();

        }
#ifdef _WIN32
        WSACleanup();
#endif
        isServerRunning=false;
    }
}
//-------------------------------------------------------------------------
bool CServer::launch(int port)
{
#ifdef _WIN32
    WSAStartup(MAKEWORD(2,0),&wsaData); //inicializuojame WS2_32.DLL
#endif
    serverSock = socket(AF_INET, SOCK_STREAM, 0);

#ifdef _WIN32
    if (serverSock == INVALID_SOCKET)
#else
    if (serverSock == -1)
#endif
    {
        printf("Invalid socket!\n");
        return false;
    }

#ifdef _WIN32
    SOCKADDR_IN sin;
#else
    sockaddr_in sin;
#endif
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(port); //nustatome prievada(port)


#ifdef _WIN32
    if (bind(serverSock,(LPSOCKADDR)&sin,sizeof(sockaddr_in))==SOCKET_ERROR)
#else
    if (bind(serverSock, (sockaddr*)&sin, sizeof(sockaddr_in)) == -1)
#endif
    {
        printf("can't bind\n");
        return false;
    }

#ifdef _WIN32
    DWORD nonBlocking = 1;
    if ( ioctlsocket(serverSock,
                     FIONBIO,
                     &nonBlocking ) != 0 )
    {
        printf( "failed to set non-blocking\n" );
        return false;
    }
#else

    int nonBlocking = 1;
    if ( fcntl(serverSock,
               F_SETFL,
               O_NONBLOCK,
               nonBlocking ) == -1 )
    {
        printf( "failed to set non-blocking\n" );
        return false;
    }


#endif

    isServerRunning = true;
    return true;
}
//-------------------------------------------------------------------------------
int CServer::waitForClient()
{
    if (isServerRunning)
    {
        int result=0;
        result = listen(serverSock, 0);

        if (result < 0)
        {
#ifdef _WIN32
            if (result == SOCKET_ERROR)
#else
            if (result == -1)
#endif
            {
                return 0;
            }
        }

        sockaddr_in addr;

#ifdef _WIN32
        int len = sizeof(sockaddr_in);
        SOCKET temporary = accept(serverSock,(SOCKADDR *)&addr,&len);
#else
        socklen_t len = sizeof(sockaddr_in);
        int temporary = accept(serverSock,(sockaddr*)&addr, &len);
#endif

#ifdef _WIN32
        if (temporary!= INVALID_SOCKET)
#else
        if (temporary != -1)
#endif
        {
            clientAddreses.add(addr);
            clientSocks.add(temporary);

            _clientCount++;

            return 1;
        }
        else
        {
            return 0; 
        }
    }

    return 0;
}
//----------------------------------------------
int CServer::getData(int clientIndex, char *buf)
{
    if (_clientCount)
    {
#ifdef _WIN32
        FD_SET set;
#else
        fd_set set;
#endif
        timeval waitTimeStr;

        FD_ZERO( &set );
        FD_SET( clientSocks[clientIndex], &set );
        waitTimeStr.tv_sec = 0;  
        waitTimeStr.tv_usec = 0;


        int res = select(FD_SETSIZE, &set, 0, 0, &waitTimeStr);

        if (res > 0)
        {
            int bytes = recv(clientSocks[clientIndex], buf, 1024, 0);
            return bytes;
        }

    }
    return 0;
}

//----------------------------------------------
void CServer::sendData(unsigned clientIndex, const char *data, int len){
    send(clientSocks[clientIndex],data,len,0);
}
//-------------------------------------------
void CServer::removeClient(unsigned index){
    _clientCount--;
#ifdef _WIN32
    closesocket(clientSocks[index]);
#else
    close(clientSocks[index]);
#endif
    clientSocks.remove(index);
    clientAddreses.remove(index);

}
