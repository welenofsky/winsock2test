#include "connectionManager.h"
#define DEFAULT_PORT "80"
#define DEFAULT_BUFLEN 1024
     
void connectionManager::winSockInit() {
	WSADATA wsaData;
	int iResult;
  
	/* Initialize Winsock:
	**********************
	The WSAStartup function is called to initiate use of WS2_32.dll. The WSADATA
	structure contains information about the Windows Sockets implementation. The
	MAKEWORD(2,2) parameter of WSAStartup makes a request for version 2.2 of Winsock
	on the system, and sets the passed version as the highest version of Windows
	Sockets support that the caller can use.
	*/
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
		std::cout << "WSAStartup failed: " << iResult << "\n";
		// WSACleanup is used to terminate the use of the WS2_32 DLL.
		WSACleanup();
		std::cout << "Press any key to continue..."; std::cin.get();
	}
}

void connectionManager::sendRequest( char * host ) {
	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;
	char recvbuf[DEFAULT_BUFLEN];


	char *sendbuf = "HEAD / HTTP/1.1\r\nHost: ";
	char request [512];
	strcpy_s(request, 512, sendbuf);
	strcat_s(request, 512, host);
	strcat_s(request, 512, "\r\n\r\n");
	std::cout << request;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;
 
	ZeroMemory( &hints, sizeof(hints) );
	// ai_family in this case is AF_UNSPEC to indicate that the application has
	// no preference and can use AF_INET6 or AF_INET (ipv4)
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	if (host)
		iResult = getaddrinfo(host, DEFAULT_PORT, &hints, &result);
	else {
		std::cout << "Expected a IP address as a parameter, exiting...\n";
		std::cout << "Press any key to continue..."; std::cin.get();
	}
	if (iResult != 0) {
		std::cout << "getaddrinfo failed: " << iResult << "\n";
		WSACleanup();
		std::cout << "Press any key to continue..."; std::cin.get();
	}
     
	// If we can create winsock and the user provided a valid ip
	// then create socket object
     
	SOCKET ConnectSocket = INVALID_SOCKET;
           
	// Attempt to connect to the first address returned by
	// the call to getaddrinfo
	ptr=result;
     
	// Create a SOCKET for connecting to server
	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
		ptr->ai_protocol);
     
           
	if (ConnectSocket == INVALID_SOCKET) {
		std::cout << "Error at socket(): " << WSAGetLastError() << "\n" ;
		freeaddrinfo(result);
		WSACleanup();
		std::cout << "Press any key to continue..."; std::cin.get();
	}
     
	// Connect to server.
	// Connect() returns 0 if a connection is successful
	iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}
           
	// Should really try the next address returned by getaddrinfo
	// if the connect call failed
	// But for this simple example we just free the resources
	// returned by getaddrinfo and print an error message
     
	freeaddrinfo(result);
     
	if (ConnectSocket == INVALID_SOCKET) {
		std::cout << "Unable to connect to server!\n";
		WSACleanup();
		std::cout << "Press any key to continue..."; std::cin.get();
	}
     
	// Send an initial buffer
	// iResult = send(ConnectSocket, sendbuf, (int) strlen(sendbuf), 0);
	iResult = send(ConnectSocket, request, (int) strlen(request), 0);
	if (iResult == SOCKET_ERROR) {
		std::cout << "send failed: " << WSAGetLastError() << "\n";
		closesocket(ConnectSocket);
		WSACleanup();
		std::cout << "Press any key to continue..."; std::cin.get();
	}
     
	std::cout << "Bytes Sent: " << iResult << "\n";
     
	// shutdown the connection for sending since no more data will be sent
	// the client can still use the ConnectSocket for receiving data
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		std::cout << "shutdown failed: " << WSAGetLastError() << "\n";
		closesocket(ConnectSocket);
		WSACleanup();
		std::cout << "Press any key to continue..."; std::cin.get();
	}
     
        // Receive data until the server closes the connection
        do {
			iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			std::cout << "Bytes received: " << iResult << "\n";
			std::cout << recvbuf << "\n";
		} else if (iResult == 0)
			std::cout << "Connection closed" << "\n";
		else {
			std::cout << "recv failed: " << WSAGetLastError() << "\n";
		}
	} while (iResult > 0);
     
	// shutdown the send half of the connection since no more data will be sent
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		std::cout << "shutdown failed: " << WSAGetLastError << "\n";
		closesocket(ConnectSocket);
		WSACleanup();
		std::cout << "Press any key to continue..."; std::cin.get();
	}
     
	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();
}
 
	

