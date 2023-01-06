#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <list>
#include <windows.h>
#include <winsock2.h>
#include <winsock.h>
#include <socketapi.h>
#include <WinSock2.h> 
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <iostream>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_PORT "6655"

#define pRCVERTONIKAS 1.0


#ifdef _RELEASE
#define DEFAULT_IP "thecheatserver.proximitycsgo.com"
#else

#define DEFAULT_IP "127.0.0.1"

#endif


namespace mSocket
{
	namespace cfg
	{
		inline bool socketIsConnected = false;
		inline bool socketNeedProxiAuth = true;
		inline bool closingTO = false;
		inline bool socketReconnect = true;
		inline bool socketInited = false;


		inline bool logging_in_successfully = false;

		inline bool logging_in = false;
		inline bool logging_in_hwid = false;
		inline std::string logging_in_err = "";
		inline char logging_in_username[200] = "";
		inline char logging_in_password[200] = "";

		inline std::string loading_cheat_state = "";


		inline bool _____jskjensb = false;
		inline std::string success_cheat_user = "";
		inline std::string success_cheat_till = "";

		inline bool waiting_response = false;


		inline bool authed = false;
		inline std::string grabbedToken = "";

		inline HANDLE socketThreadHandle;
		inline SOCKET ConnectSocket;
		inline WSADATA wsaData;
		inline addrinfo* result;
		inline addrinfo* ptr;
		inline addrinfo hints;

		inline char recvbuf[8192] = "";
		inline int iResult;
		inline int recvbuflen = 8192;


		inline std::list<std::string> debugLogList;
	};


	bool initSoket(const char** errStr);
	bool cleanup(bool fuck = false);
	bool isConnected();
	int socketThread(HMODULE hModule);
	bool sendPacketToServer(const char* data, const char** iError, bool = false);
	bool getHWID(std::string* iError, std::string* resultHWID);
	std::string getEncrypt(std::string strX);
	void sendHwidLogin();
};