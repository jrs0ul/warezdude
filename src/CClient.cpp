#include "CClient.h"

/*Client class
 jrs0ul
 2007 03 23
*/

bool CClient::connectServer(const char* ip, unsigned port){
	WSAData wsaData;
	WSAStartup(MAKEWORD(2,0),&wsaData);
	clientSock = socket(AF_INET,SOCK_STREAM,0);
	if (clientSock == INVALID_SOCKET) 
		return false;
   
	LPHOSTENT hostEntry;
	hostEntry = gethostbyname(ip);
	if (!hostEntry)
		return false;
   
	_sin.sin_family = AF_INET;
	_sin.sin_addr = *((LPIN_ADDR)*hostEntry->h_addr_list);
    _sin.sin_port = htons(port);
   
	if (connect(clientSock,(LPSOCKADDR)&_sin, sizeof(sockaddr_in))!= SOCKET_ERROR){
	 isJoinedServer=true;
     return true;
	}
	return false;
}



void CClient::disconnectServer(){
	if (isJoinedServer){
		closesocket(clientSock);
		WSACleanup();
		isJoinedServer=false;
	}
}


void CClient::sendData(const char *data, int len){
	send(clientSock,data,len,0);
}


int CClient::getData(char* data){
	FD_SET   set;
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