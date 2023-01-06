

namespace Packets
{
	namespace NServerPackets
	{
		enum EFromServerToClient
		{
			USER_AUTH_RESPONSE,
			HWID_AUTH_RESPONSE,
			VERSION_RESP,
		};
	};

	namespace NClientPackets
	{
		enum EFromClientToServer
		{
			USER_AUTH,
			HWID_AUTH,
		};
	};
};