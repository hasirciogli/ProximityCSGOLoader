#include "DataHandler.h"
#include "Packet.h"
#include "../msoket.h"
//#include "../../core/menu/chatbox/ChatBox.h"
//#include "dependencies/imgui/imgui.h"

using namespace nlohmann;


void CDataHandlerFuncs::NeedUserAuth(std::string fullData)
{
	json j = json();

	j["packet_id"] = Packets::NClientPackets::USER_AUTH;
	j["data"]["hwid"] = "9963"; // Your hwid data

	const char* sError = "";

	mSocket::sendPacketToServer(j.dump().c_str(), &sError);
}

void CDataHandlerFuncs::UserAuth(std::string fullData)
{

	std::cout << "My data: " << fullData << std::endl;

	json faj = json::parse(fullData);
	int packetID = faj["packet_id"];//faj["packet_id"];
	std::string dataSTR = faj["data"].dump();
	bool isSuccess = faj["data"]["isSuccess"];
	std::string token = faj["data"]["token"];

	std::cout << "My data: " << dataSTR << std::endl;

	std::cout << "success: " << isSuccess << std::endl;
	std::cout << "token: " << token << std::endl;

	if (isSuccess)
	{
		if (token != "")
		{
			mSocket::cfg::authed = true;
			mSocket::cfg::grabbedToken = token;
		}
		else
		{
			mSocket::cfg::authed = false;
			mSocket::cfg::grabbedToken = "";
		}
	}
	else
	{
		mSocket::cfg::authed = false;
		mSocket::cfg::grabbedToken = "";
	}

	if (!mSocket::cfg::authed)
		mSocket::cleanup();
}