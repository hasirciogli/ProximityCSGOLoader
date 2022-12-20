#include "DataHandler.h"
#include "Packet.h"

using namespace nlohmann::json_abi_v3_11_2;

void CDataHandler::Handle()
{
	try
	{
		json jbbk;
		jbbk = json::parse(this->data);
	

		int packetID = jbbk["packet_id"];

		if (!packetID == 0 && packetID == NULL)
			return;

		CDataHandlerFuncs cdhf = CDataHandlerFuncs();

		switch ((Packets::NServerPackets::EFromServerToClient)packetID)
		{
			
		case Packets::NServerPackets::EFromServerToClient::NEED_USER_AUTH:
			cdhf.NeedUserAuth(this->data);
			break;

		case Packets::NServerPackets::EFromServerToClient::USER_AUTH_RESPONSE:
			cdhf.UserAuth(this->data);
			break;

			default:
				break;
		}
	}
	catch (json::parse_error& ex)
	{
		printf("json parse error = %i\n\n", ex.byte);
	}
}