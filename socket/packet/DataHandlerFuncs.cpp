#include "DataHandler.h"
#include "Packet.h"
#include "../msoket.h"
//#include "../../core/menu/chatbox/ChatBox.h"
//#include "dependencies/imgui/imgui.h"

using namespace nlohmann;


bool CDataHandlerFuncs::UserLoginResponse(std::string pData)
{
	json jsp = json();

	try
	{
		jsp = json::parse(pData)["data"];

		bool   isSuccess = jsp["isSuccess"];
		bool   isBanned = jsp["isBanned"];
		bool   subs_active = jsp["subs_active"];
		bool   need_hwid_reset = jsp["need_hwid_reset"];
		std::string uBanReason = jsp["uBanReason"];
		std::string username = jsp["username"];
		std::string subs_till = jsp["subs_till"];

		if (isSuccess)
		{
			if (isBanned)
			{
				mSocket::cfg::logging_in_successfully = false;
				mSocket::cfg::logging_in_hwid = false;
				mSocket::cfg::logging_in = false;
				mSocket::cfg::logging_in_err = "Ban: " + uBanReason;
			}
			else
			{
				if (need_hwid_reset)
				{
					mSocket::cfg::logging_in_successfully = false;
					mSocket::cfg::logging_in_hwid = false;
					mSocket::cfg::logging_in = false;
					mSocket::cfg::logging_in_err = "You need to reset your hwid";

					ShellExecute(NULL, "open", "http://proximitycsgo.com/", NULL, NULL, SW_SHOWNORMAL);
				}
				else if (!subs_active)
				{
					mSocket::cfg::logging_in_successfully = false;
					mSocket::cfg::logging_in_hwid = false;
					mSocket::cfg::logging_in = false;
					mSocket::cfg::logging_in_err = "Bruh you dont have an active sub :(";
				}
				else
				{
					mSocket::cfg::success_cheat_user = username;
					mSocket::cfg::logging_in_successfully = true;
				}
			}
		}
		else
		{
			mSocket::cfg::logging_in_successfully = false;
			mSocket::cfg::logging_in_hwid = false;
			mSocket::cfg::logging_in = false;
			mSocket::cfg::logging_in_err = "Invalid Username Or Password...";
		}
	}
	catch (json::parse_error& pErr)
	{
		mSocket::cfg::debugLogList.push_back("Parse Error");
		mSocket::cfg::debugLogList.push_back(pErr.what());
		mSocket::cfg::debugLogList.push_back(std::to_string(pErr.id));
	}

	mSocket::cfg::logging_in_hwid = false;

	return false;
}

bool CDataHandlerFuncs::HwidLoginResponse(std::string pData)
{
	json jsp = json();

	try
	{
		jsp = json::parse(pData)["data"];

		bool   isSuccess			= jsp["isSuccess"];
		bool   isBanned				= jsp["isBanned"];
		std::string username		= jsp["username"];
		std::string uBanReason		= jsp["uBanReason"];
		bool subs_active			= jsp["subs_active"];
		std::string subs_till		= jsp["subs_till"];

		if (isSuccess)
		{
			if (isBanned)
			{
				mSocket::cfg::logging_in_successfully = false;
				mSocket::cfg::logging_in_hwid = false;
				mSocket::cfg::logging_in = false;
				mSocket::cfg::logging_in_err = "You got banned from PROXIMITY";
			}
			else
			{
				if (!subs_active)
				{
					mSocket::cfg::logging_in_successfully = false;
					mSocket::cfg::logging_in_hwid = false;
					mSocket::cfg::logging_in = false;
					mSocket::cfg::logging_in_err = "Bruh you dont have an active sub :(";
				}
				else
				{
					mSocket::cfg::success_cheat_user = username;
					mSocket::cfg::logging_in_successfully = true;
					mSocket::cfg::success_cheat_till = subs_till;
				}
			}
		}
		else
		{
			mSocket::cfg::logging_in_successfully = false;
			mSocket::cfg::logging_in_hwid = false;
			mSocket::cfg::logging_in = false;
		}
	}
	catch (json::parse_error& pErr)
	{
		mSocket::cfg::debugLogList.push_back("Parse Error");
		mSocket::cfg::debugLogList.push_back(pErr.what());
		mSocket::cfg::debugLogList.push_back(std::to_string(pErr.id));
	}
	return false;
}
