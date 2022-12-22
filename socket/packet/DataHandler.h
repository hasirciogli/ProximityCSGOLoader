#include <string>
#include <iostream>

#include <nlohmann/json.hpp>

class CDataHandler
{
public:
	std::string data = "";

	void Handle();
};

class CDataHandlerFuncs
{
public:
	bool UserLoginResponse(std::string pData);
	bool HwidLoginResponse(std::string pData);
	bool CheatResponse(std::string pData);
};