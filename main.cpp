	

    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
     
    #include <Windows.h>
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <iostream>
     
    #define DEFAULT_PORT "80"

	// 1024, up from 512. Was getting gibberish unicode chars
	// with 512 byte buffer limit. !!Need to look into this!!
    #define DEFAULT_BUFLEN 1024
     
    #pragma comment(lib, "Ws2_32.lib")
     
    void anyKey(){ std::cout << "Press any key to continue..."; std::cin.get(); }
     
    int main (int argc, char* argv[]) {
           
            // Create of a winsock data object
            WSADATA wsaData;
     
            int iResult;
            int recvbuflen = DEFAULT_BUFLEN;
            // Holds the status of the internet connection
            bool isUp = false;
            if (sizeof(argv[1]) > 255) return 1;
            char *host = argv[1];
            // Get request, doesn't seem to be formal. Always invokes
            // a bad request response from servers
            char *sendbuf = "HEAD / HTTP/1.1\r\nHost: ";
            char request [512];
            strcpy_s(request, 512, sendbuf);
            strcat_s(request, 512, argv[1]);
            strcat_s(request, 512, "\r\n\r\n");
            std::cout << request;
            char recvbuf[DEFAULT_BUFLEN];
     
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
                    anyKey();
                    return 1;
            }
     
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
            if (argv[1])
                    iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
            else {
                    std::cout << "Expected a IP address as a parameter, exiting...\n";
                    anyKey();
                    return 1;
            }
            if (iResult != 0) {
                    std::cout << "getaddrinfo failed: " << iResult << "\n";
                    WSACleanup();
                    anyKey();
                    return 1;
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
                    anyKey();
                    return 1;
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
                    anyKey();
                    return 1;
            }
     
            // Send an initial buffer
            // iResult = send(ConnectSocket, sendbuf, (int) strlen(sendbuf), 0);
            iResult = send(ConnectSocket, request, (int) strlen(request), 0);
            if (iResult == SOCKET_ERROR) {
                    std::cout << "send failed: " << WSAGetLastError() << "\n";
                    closesocket(ConnectSocket);
                    WSACleanup();
                    anyKey();
                    return 1;
            }
     
            std::cout << "Bytes Sent: " << iResult << "\n";
     
            // shutdown the connection for sending since no more data will be sent
            // the client can still use the ConnectSocket for receiving data
            iResult = shutdown(ConnectSocket, SD_SEND);
            if (iResult == SOCKET_ERROR) {
                    std::cout << "shutdown failed: " << WSAGetLastError() << "\n";
                    closesocket(ConnectSocket);
                    WSACleanup();
                    anyKey();
                    return 1;
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
                    anyKey();
                    return 1;
            }
     
            // cleanup
            closesocket(ConnectSocket);
            WSACleanup();
     
            anyKey();
            return 0;
    }

