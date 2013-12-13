	

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
     
#include "winsock2test\connectionManager.h"
     
     
#pragma comment(lib, "Ws2_32.lib")
     
void anyKey(){ std::cout << "Press any key to continue..."; std::cin.get(); }
     
int main (int argc, char* argv[]) {
	// Holds the status of the internet connection
	bool isUp = false;
	if (sizeof(argv[1]) > 255) return 1;
	char *host = argv[1];
	connectionManager cm;
	// Get request, doesn't seem to be formal. Always invokes
	// a bad request response from servers
	cm.winSockInit();
	cm.sendRequest(host);

	anyKey();
	return 0;
}		

