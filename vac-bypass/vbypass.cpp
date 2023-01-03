#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define dwClientState 0x59F194
#define dwClientState_State  0x108

#include <Windows.h>
#include <urlmon.h>
#include "bypassByte.h"
#include "VBMem.h"
#include "vbypass.h"

#include <filesystem>
#include <functional>
#include <tchar.h>

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

class ProcessMemory
{
public:
    ProcessMemory(const std::string& process_name)
    {
        // Bu fonksiyon, belirtilen iþlem adýna göre bir iþlem ID'si döndürür.
        m_process_id = get_process_id(process_name);

        // Bu fonksiyon, belirtilen iþlem ID'sine sahip iþlemin bellek
        // eriþim haklarýný alýr.
        m_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_process_id);
        if (!m_handle) {
            throw std::runtime_error("Failed to open process: " + process_name);
        }
    }

    ~ProcessMemory()
    {
        CloseHandle(m_handle);
    }

    template <typename T>
    T read(DWORD address)
    {
        T value;
        ReadProcessMemory(m_handle, reinterpret_cast<void*>(address), &value, sizeof(T), nullptr);
        return value;
    }

    template <typename T>
    void write(DWORD address, T value)
    {
        WriteProcessMemory(m_handle, reinterpret_cast<void*>(address), &value, sizeof(T), nullptr);
    }

private:
    DWORD get_process_id(const std::string& process_name)
    {
        DWORD process_id = 0;
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot != INVALID_HANDLE_VALUE) {
            PROCESSENTRY32 process_entry;
            process_entry.dwSize = sizeof(PROCESSENTRY32);
            if (Process32First(snapshot, &process_entry)) {
                do {
                    if (process_name == process_entry.szExeFile) {
                        process_id = process_entry.th32ProcessID;
                        break;
                    }
                } while (Process32Next(snapshot, &process_entry));
            }
            CloseHandle(snapshot);
        }
        return process_id;
    }

public:

    HANDLE m_handle;
    DWORD m_process_id;
};

DWORD dwGetModuleBaseAddress(DWORD dwProcessID, TCHAR* lpszModuleName)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessID);
    DWORD dwModuleBaseAddress = 0;
    if (hSnapshot != INVALID_HANDLE_VALUE)
    {
        MODULEENTRY32 ModuleEntry32 = { 0 };
        ModuleEntry32.dwSize = sizeof(MODULEENTRY32);
        if (Module32First(hSnapshot, &ModuleEntry32))
        {
            do
            {
                if (_tcscmp(ModuleEntry32.szModule, lpszModuleName) == 0)
                {
                    dwModuleBaseAddress = (DWORD)ModuleEntry32.modBaseAddr;
                    break;
                }
            } while (Module32Next(hSnapshot, &ModuleEntry32));
        }
        CloseHandle(hSnapshot);
    }
    return dwModuleBaseAddress;
}

void BypassLoader::LoaderLoop(HMODULE hModule)
{
    while (!mSocket::cfg::_____jskjensb)
        Sleep(200);

    mSocket::cfg::loading_cheat_state = "Cheat loading has been started";
    /*ProcessMemory mem("csgo.exe");

    while (!GetAsyncKeyState(VK_SPACE))
    {
        auto EngineDLL = dwGetModuleBaseAddress(mem.m_process_id, (TCHAR*)"engine.dll");
        auto EnginePointer = mem.read<DWORD>(EngineDLL + dwClientState);
        auto GameState = mem.read<int>(EnginePointer + dwClientState_State);

	    if (true)
	    {
            mSocket::cfg::loading_cheat_state = std::to_string(GameState);
            mSocket::cfg::debugLogList.push_front(std::to_string(GameState));
	    }
        Sleep(300);
    }
    exit(0);*/

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

    mSocket::cfg::loading_cheat_state = "Csgo Process found... (Waiting Window...)";


    while (!FindWindowA(0, "Counter-Strike: Global Offensive - Direct3D 9"))
        Sleep(2000);

    Sleep(3000);
    mSocket::cfg::loading_cheat_state = "Loading FLB States (Kernel Bypass Funtions)";

    Sleep(2000);
    mSocket::cfg::loading_cheat_state = "Injecting FLB...";

    Sleep(3000);
    mSocket::cfg::loading_cheat_state = "Checking Bypass Lib's (Kernel Bypass Checking)";

    Sleep(2000);
    mSocket::cfg::loading_cheat_state = "Bypass WORK!";

    Sleep(3000);
    mSocket::cfg::loading_cheat_state = "Loading Proximity (Kernel Process Lock)";

    Sleep(16000);

    //LibraryLoader::inject(LibLoaderFunc::FindProcessId("csgo.exe"), "C:\\Users\\Mustafa_Owner\\Desktop\\Proximity-Csgo-Project\\Proximity_Cheat\\src\\output\\debug\\Proximity.dll");

    LibLoaderFunc::LoadLib("csgo.exe");
    mSocket::cfg::loading_cheat_state = "PROXIMITY IS LOADED LETS F*CKING GO!";

    Sleep(5000);    

    exit(0);
}