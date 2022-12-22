

namespace Packets
{
	namespace NServerPackets
	{
		enum EFromServerToClient
		{
			USER_AUTH_RESPONSE,
			HWID_AUTH_REPONSE,
			CHEAT_RESPONSE
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