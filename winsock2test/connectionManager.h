#ifndef CONNECTION_MANAGER_H
#define CONNECTION_MANAGER_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

class connectionManager {
public:
	void winSockInit();
	void sendRequest( char * host );
private:
};
#endif