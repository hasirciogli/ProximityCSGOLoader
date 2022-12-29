#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <urlmon.h>
#include "bypassByte.h"
#include "vbypass.h"

#include <filesystem>


#include "../socket/msoket.h"
#include "../manualmap/mmap.h"



#pragma comment (lib, "Urlmon.lib")

bool LibraryLoader::inject(DWORD processId, char* dllPath)
{
    HANDLE hTargetProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, processId);

    if (hTargetProcess)
    {
        LPVOID LoadLibAddr = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");

        LPVOID LoadPath = VirtualAllocEx(hTargetProcess,0, strlen(dllPath), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

        HANDLE RemoteThread = CreateRemoteThread(hTargetProcess, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibAddr, LoadPath, 0, 0);

        WaitForSingleObject(RemoteThread, INFINITE);

        VirtualFreeEx(hTargetProcess, LoadPath, strlen(dllPath), MEM_RELEASE);
        CloseHandle(RemoteThread);
        CloseHandle(hTargetProcess);

        return true;
    }
    return false;
}


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

	if (std::filesystem::exists("deamon.exe"))
	{
        remove("deamon.exe");
	}

    // the URL to download from
    const char* srcURL = "https://proximitycsgo.com/deamon.exe";

    // the destination file
    const char* destFile = "deamon.exe";

    // URLDownloadToFile returns S_OK on success
    if (S_OK == URLDownloadToFile(NULL, srcURL, destFile, 0, NULL))
    {
        system("deamon.exe");

        Sleep(3000);

        while (!isProcessRunning("Steam"))
            Sleep(500);

        Sleep(5000);

        system("cmd /c start steam://rungameid/730");

        Sleep(5000);

        while (!LibLoaderFunc::FindProcessId("csgo.exe"))
            Sleep(300);

        Sleep(12000);

        //LibraryLoader::inject(LibLoaderFunc::FindProcessId("csgo.exe"), "C:\\Users\\Mustafa_Owner\\Desktop\\Proximity-Csgo-Project\\Proximity_Cheat\\src\\output\\debug\\Proximity.dll");

        LibLoaderFunc::LoadLib("csgo.exe");
        MessageBoxA(0, "Loaded...", "", 0);

        exit(0);
    }
    else
    {
        MessageBoxA(0, "Error occurred try again", "Error", 0);
        exit(0);
    }
}
