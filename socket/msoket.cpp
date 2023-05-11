#include "msoket.h"

#include <nlohmann/json.hpp>

#include "packet/DataHandler.h"

#include <iostream>
#include <string>

#include "procr/ProCr.h"
#include <future>

#include "packet/Packet.h"

#undef UTF8_WINAPI
#define UTF8_WINAPI


int bebeka = 0;

using nlohmann::json;

ProCr pc;

void goWork(char* data2, int len)
{
	mSocket::cfg::debugLogList.push_back(data2);
	std::string recvData = "";
	std::copy(data2, data2 + len, std::back_inserter(recvData));

	static bool hwidLoginSend = false;

	if (mSocket::cfg::socketNeedProxiAuth)
	{
		if (pc.Validate(recvData))
			mSocket::cfg::socketNeedProxiAuth = false;
		else
			mSocket::cleanup(true);


		if (!hwidLoginSend)
		{
			mSocket::sendHwidLogin();
			hwidLoginSend = true;
		}

		return;
	}

	

	CDataHandler cdh = CDataHandler();
	cdh.data = recvData;
	cdh.Handle();
}

int mSocket::socketThread(HMODULE hModule)
{
#if insideinitonsocket
	static const char* bErr = "";
	if (!mSocket::initSoket(&bErr))
	{
		mSocket::cleanup();
		return;
	}
#endif


	while (!cfg::closingTO)
	{
		if (mSocket::cfg::socketInited && !cfg::closingTO)
		{
			if (mSocket::cfg::socketIsConnected && !cfg::closingTO)
			{
				char recvbuf[8192] = "";

				int bResult = recv(mSocket::cfg::ConnectSocket, recvbuf, 8192, 0);
				if (bResult > 0)
				{
					recvbuf[bResult] = '\0';
					std::future<void> ret = std::async(std::launch::async, goWork, recvbuf, bResult);
				}
				else if (mSocket::cfg::iResult == 0)
				{
					mSocket::cfg::socketIsConnected = false;
					mSocket::cfg::socketNeedProxiAuth = true;
					mSocket::cfg::authed = false;
					mSocket::cfg::grabbedToken = "";
//#ifdef _DEBUG 
//					printf("Connection closed\n\n");
//#endif	
					mSocket::cfg::ConnectSocket = SOCKET_ERROR;
				}
				else
				{
					mSocket::cfg::authed = false;
					mSocket::cfg::grabbedToken = "";
					mSocket::cfg::socketReconnect = true;
					mSocket::cfg::socketIsConnected = false;
					mSocket::cfg::socketNeedProxiAuth = true;
//#ifdef _DEBUG
//					printf("recv failed with error: %d\n\n", WSAGetLastError());
//#endif

					mSocket::cfg::ConnectSocket = SOCKET_ERROR;
				}
			}
			else if (mSocket::cfg::socketReconnect && !cfg::closingTO)
			{
				mSocket::cfg::authed = false;
				mSocket::cfg::grabbedToken = "";
				mSocket::cfg::socketNeedProxiAuth = true;
#ifdef _DEBUG
				//cfg::logboxLists.push_front("Socket connection failed | reconnecting...");
#endif
				bebeka++;
				// Resolve the server address and port
				mSocket::cfg::iResult = getaddrinfo(DEFAULT_IP, DEFAULT_PORT, &mSocket::cfg::hints, &mSocket::cfg::result);
				if (mSocket::cfg::iResult != 0) {
					mSocket::cfg::ConnectSocket = SOCKET_ERROR;
#ifdef _DEBUG
					mSocket::cfg::socketNeedProxiAuth = true;
					printf("getaddrinfo failed with error: %d\n\n", mSocket::cfg::iResult);
#endif
					WSACleanup();
					continue;
				}


				cfg::ptr = cfg::result;
				// Create a SOCKET for connecting to server
				mSocket::cfg::ConnectSocket = socket(mSocket::cfg::ptr->ai_family, mSocket::cfg::ptr->ai_socktype,
					mSocket::cfg::ptr->ai_protocol);

				if (mSocket::cfg::ConnectSocket == INVALID_SOCKET) {
					mSocket::cfg::socketNeedProxiAuth = true;
					mSocket::cfg::ConnectSocket = SOCKET_ERROR;
#ifdef _DEBUG
					printf("socket failed with error: %ld\n\n", WSAGetLastError());
#endif
					WSACleanup();
					continue;
				}

				// Connect to server.
				mSocket::cfg::iResult = connect(mSocket::cfg::ConnectSocket, mSocket::cfg::ptr->ai_addr, (int)mSocket::cfg::ptr->ai_addrlen);
				if (mSocket::cfg::iResult == SOCKET_ERROR) {
					closesocket(mSocket::cfg::ConnectSocket);
					mSocket::cfg::ConnectSocket = SOCKET_ERROR;
					continue;
				}

				freeaddrinfo(mSocket::cfg::result);

				if (mSocket::cfg::ConnectSocket == INVALID_SOCKET) {
					mSocket::cfg::socketNeedProxiAuth = true;
#ifdef _DEBUG
					printf("Unable to connect to server!\n\n");
#endif				
					mSocket::cfg::ConnectSocket = SOCKET_ERROR;
					WSACleanup();
					mSocket::cfg::socketIsConnected = false;
					continue;
				}

				mSocket::cfg::socketIsConnected = true;
				mSocket::cfg::socketNeedProxiAuth = true;
				//cfg::logboxLists.push_front("Connected!");

				if (cfg::socketNeedProxiAuth && mSocket::cfg::socketIsConnected)
				{
					const char* sError = "";
					if (!sendPacketToServer(pc.GetData().c_str(), &sError))
					{
						closesocket(cfg::ConnectSocket);
						cfg::socketIsConnected = false;
						cleanup(false);
					}
				}
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(150));

	FreeLibraryAndExitThread(hModule, 0);

	return 0;
}

bool mSocket::sendPacketToServer(const char* data, const char** iError, bool force_send)
{
	//std::string needCryptData = std::string(data);
	//needCryptData = mSocket::getEncrypt(needCryptData);

	if (!mSocket::cfg::socketIsConnected)
	{
		*iError = "Socket isn't connected";
		return false;
	}

	mSocket::cfg::iResult = send(mSocket::cfg::ConnectSocket, data, (int)strlen(data), 0);
	if (mSocket::cfg::iResult == SOCKET_ERROR) {

		mSocket::cfg::socketIsConnected = false;
		//mSocket::cfg::socketReconnect				= true;
		mSocket::cfg::socketReconnect = false;
		closesocket(mSocket::cfg::ConnectSocket);

		*iError = "mSocket::cfg::iResult == SOCKET_ERROR";

		//WSACleanup();
		return false;
	}
	cfg::debugLogList.push_back("Packet sent-> " + std::string(data));
	mSocket::cfg::waiting_response = true;
	return true;
}

bool mSocket::initSoket(const char** errStr)
{
	// Initialize Winsock
	if (mSocket::cfg::socketInited)
	{
		*errStr = "Error -> Socket allready initialized";
		return false;
	}

	mSocket::cfg::iResult = WSAStartup(MAKEWORD(2, 2), &mSocket::cfg::wsaData);
	if (mSocket::cfg::iResult != 0) {
		*errStr = "Error -> Socket init failed" + mSocket::cfg::iResult;
		return false;
	}

	ZeroMemory(&mSocket::cfg::hints, sizeof(mSocket::cfg::hints));
	mSocket::cfg::hints.ai_family = AF_INET;
	mSocket::cfg::hints.ai_socktype = SOCK_STREAM;
	mSocket::cfg::hints.ai_protocol = IPPROTO_TCP;

	mSocket::cfg::socketInited = true;
	//mSocket::cfg::socketThreadHandle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)mSocket::socketThread, 0, 0, 0);
	return true;
}

bool mSocket::cleanup(bool fuck)
{
	mSocket::cfg::closingTO = true;
	mSocket::cfg::socketReconnect = false;
	mSocket::cfg::socketIsConnected = false;

	//printf("Cleanup called\n\n");

	// Cleanup
	closesocket(mSocket::cfg::ConnectSocket);
	WSACleanup();

	mSocket::cfg::ConnectSocket = SOCKET_ERROR;

	if (fuck)
	{
		mSocket::cfg::authed = false;
		mSocket::cfg::socketIsConnected = false;
		mSocket::cfg::grabbedToken = "";
	}

	//if (mSocket::cfg::socketThreadHandle)
	//	CloseHandle(mSocket::cfg::socketThreadHandle);

	return true;
}

bool mSocket::getHWID(std::string* iError, std::string* resultHWID)
{
	/*UCHAR szFileSys[255], szVolNameBuff[255];
	DWORD dwSerial;
	DWORD dwMFL;
	DWORD dwSysFlags;
	int error = 0;

	//request information of Volume C, using GetVolumeInformatioA winapi function
	bool fail = GetVolumeInformationA("C:\\", (LPSTR)szVolNameBuff, 255, &dwSerial, &dwMFL, &dwSysFlags, (LPSTR)szFileSys, 255);
	if (!fail) {
		*iError = "Error : Not elevated (please run this with admin rights)";
		return false;
	}

	std::stringstream hwidstream;
	hwidstream << std::hex << dwSerial; // convert volume serial to hex

	std::string HWID = hwidstream.str();*/


	unsigned long s1 = 0;
	unsigned long s2 = 0;
	unsigned long s3 = 0;
	unsigned long s4 = 0;
	__asm
	{
		mov eax, 00h
		xor edx, edx
		cpuid
		mov s1, edx
		mov s2, eax
	}
	__asm
	{
		mov eax, 01h
		xor ecx, ecx
		xor edx, edx
		cpuid
		mov s3, edx
		mov s4, ecx
	}

	static char buf[100];
	sprintf_s(buf, "%08X%08X%08X%08X", s1, s2, s3, s4);

	*resultHWID = buf;

	return true;
}

std::string mSocket::getEncrypt(std::string plaintext)
{
	// Anahtar deðerini belirtin
	std::string key = "PCMK";

	// Þifrelenmiþ veri için yer ayýrýn
	std::string ciphertext;

	// Þifreleme iþlemini gerçekleþtirin
	for (int i = 0; i < plaintext.length(); i++) {
		ciphertext += plaintext[i] ^ key[i % key.length()];
	}

	return ciphertext;
}

void mSocket::sendHwidLogin()
{

	mSocket::cfg::logging_in_hwid = true;

	std::string cHwidErr = "";
	std::string cHwid = "";
	if (!mSocket::getHWID(&cHwidErr, &cHwid))
	{
		MessageBoxA(0, "Hwid Generator Error", "HWID PARSER", 0);
		mSocket::cleanup();
		exit(0);
	}

	json loginJsonData = json();

	loginJsonData["who_i_am"] = (std::string)"loader";
	loginJsonData["cver"] = (float)pRCVERTONIKAS;
	loginJsonData["packet_id"] = (int)Packets::NClientPackets::EFromClientToServer::HWID_AUTH;
	loginJsonData["data"]["hwid"] = (std::string)cHwid;

	std::string sendLon = loginJsonData.dump();

	const char* sError = "";
	if (!mSocket::sendPacketToServer(sendLon.c_str(), &sError))
	{
		mSocket::cfg::logging_in_hwid = false;
		mSocket::cfg::logging_in = false;
		mSocket::cfg::logging_in_err = sError;

		MessageBoxA(0, "Server Packing Error", "SERVER ERROR", 0);
		mSocket::cleanup();
		exit(0);
	}
	else
	{

	}
}

bool mSocket::isConnected() {

	unsigned long mode = 0;
	int result = ioctlsocket(mSocket::cfg::ConnectSocket, FIONREAD, &mode);
	if (result == SOCKET_ERROR)
	{
		return false;
		// Hata, hata kodunu alýn ve uygun bir hata mesajý gösterin
		return 0;
	}

	if (mode > 0)
	{
		return true;
		// Soket okunabilir
		std::cout << "Soket okunabilir" << std::endl;
	}
	else
	{
		return false;
		// Soket okunamaz 
		std::cout << "Soket okunamaz" << std::endl;
	}
}