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
			

			default:
				break;
		}
	}
	catch (json::parse_error& ex)
	{
		printf("json parse error = %i\n\n", ex.byte);
	}
}
