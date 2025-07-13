#include "CServer.h"
//---------------------------------------------------------------
void CServer::shutDown(){

	if (isServerRunning){
		closesocket(serverSock);
		if (_clientCount){
			for (unsigned int i=0;i<_clientCount;i++)
				closesocket(clientSocks[i]);

			clientSocks.destroy();
			clientAddreses.destroy();
			
		}
		
		WSACleanup();
		isServerRunning=false;
	}
}
//-------------------------------------------------------------------------
bool CServer::launch(int port){
	WSAStartup(MAKEWORD(2,0),&wsaData); //inicializuojame WS2_32.DLL
	serverSock = socket(AF_INET,SOCK_STREAM,0);//sukuriame serverio lizdà(socket)
	if (serverSock == INVALID_SOCKET)  return false; 

	SOCKADDR_IN sin;
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(port); //nustatome prievada(port)

	//priskiriame lizdà prievadui 
	if (bind(serverSock,(LPSOCKADDR)&sin,sizeof(sockaddr_in))==SOCKET_ERROR)
		return false;
	
	isServerRunning=true;
	return true;
}
//-------------------------------------------------------------------------------
int CServer::waitForClient(){
	if (isServerRunning){
		
		int result=0;
		result=listen(serverSock, 0);
		if (result<0)
			if (result==SOCKET_ERROR)
				return 0;

		sockaddr_in addr;
		int len=sizeof(sockaddr_in);
		SOCKET temporary=accept(serverSock,(SOCKADDR *)&addr,&len); 


		if (temporary!= INVALID_SOCKET){
			
			clientAddreses.add(addr);
			
			clientSocks.add(temporary);

			_clientCount++;

			return 1;
		}
		else{
			
			return 0; 
		}
	}




	return 0;
}
//----------------------------------------------
int CServer::getData(int clientIndex, char *buf){
	if (_clientCount){
		FD_SET   set;
		timeval     waitTimeStr;

		FD_ZERO( &set );
		FD_SET( clientSocks[clientIndex], &set );
		waitTimeStr.tv_sec = 0;  
		waitTimeStr.tv_usec = 0;


		int res=select(FD_SETSIZE,&set,0,0, &waitTimeStr);
		if (res>0){
   
			int bytes=recv(clientSocks[clientIndex],buf,1024,0);
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
	closesocket(clientSocks[index]);
	clientSocks.remove(index);
	clientAddreses.remove(index);

}