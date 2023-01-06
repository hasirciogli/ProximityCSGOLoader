#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <string>
#include <iostream>

#include "Windows.h"
#include <shellapi.h>

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
	void VersionResponse(std::string fullData);
};