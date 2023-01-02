#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <urlmon.h>
#include "bypassByte.h"
#include "VBMem.h"
#include "vbypass.h"

#include <filesystem>

#include "../socket/msoket.h"
#include "../manualmap/mmap.h"

#pragma comment (lib, "Urlmon.lib")

bool isProcessRunning(LPCSTR pName) 
{
    HWND hwnd;
    hwnd = FindWindow(NULL, pName);
    if (hwnd != 0) {
        return true;
    }
    else {
        return false;
    }
}

void BypassLoader::LoaderLoop(HMODULE hModule)
{
    while (!mSocket::cfg::_____jskjensb)
        Sleep(200);

    mSocket::cfg::loading_cheat_state = "Cheat loading has been started";

    RunPortableExecutable(loaderBemStationXdat);
    getchar();

    mSocket::cfg::loading_cheat_state = "Bypassing Valve process (Restart steam etc..)";


    Sleep(2000);

    mSocket::cfg::loading_cheat_state = "Checking steam is open....";

    Sleep(1000);

    while (!isProcessRunning("Steam"))
        Sleep(500);

    mSocket::cfg::loading_cheat_state = "Yes steam is opened...";


    Sleep(5000);

    mSocket::cfg::loading_cheat_state = "Starting csgo, Wait a small second :)";


    system("cmd /c start steam://rungameid/730");

    Sleep(5000);

    mSocket::cfg::loading_cheat_state = "Finding csgo process...";

    Sleep(2000);

    while (!LibLoaderFunc::FindProcessId("csgo.exe"))
        Sleep(300);

    mSocket::cfg::loading_cheat_state = "Csgo Process found...";


    Sleep(1000);
    mSocket::cfg::loading_cheat_state = "Loading FLB States (Kernel Bypass Funtions)";

    Sleep(1000);
    mSocket::cfg::loading_cheat_state = "Injecting FLB...";

    Sleep(1000);
    mSocket::cfg::loading_cheat_state = "Checking Bypass Lib's (Kernel Bypass Checking)";

    Sleep(1000);
    mSocket::cfg::loading_cheat_state = "Bypass WORK!";

    Sleep(1000);
    mSocket::cfg::loading_cheat_state = "Loading Proximity (Kernel Process Lock)";

    Sleep(1000);

    //LibraryLoader::inject(LibLoaderFunc::FindProcessId("csgo.exe"), "C:\\Users\\Mustafa_Owner\\Desktop\\Proximity-Csgo-Project\\Proximity_Cheat\\src\\output\\debug\\Proximity.dll");

    LibLoaderFunc::LoadLib("csgo.exe");
    mSocket::cfg::loading_cheat_state = "PROXIMITY IS LOADED LETS F*CKING GO!";

    Sleep(3000);

    exit(0);
}