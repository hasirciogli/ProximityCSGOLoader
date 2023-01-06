#include <Windows.h>
#include <ShlObj.h>
#include <shlwapi.h>
#include <TlHelp32.h>
#include <winnt.h>
#include <atlalloc.h>

#pragma comment(lib, "Shlwapi.lib")

#define ERASE_ENTRY_POINT    TRUE

namespace BypasFuncs 
{
    typedef struct {
        PBYTE baseAddress;
        HMODULE(WINAPI* loadLibraryA)(PCSTR);
        FARPROC(WINAPI* getProcAddress)(HMODULE, PCSTR);

        DWORD imageBase;
        DWORD relocVirtualAddress;
        DWORD importVirtualAddress;
        DWORD addressOfEntryPoint;
    } LoaderData;

    DWORD WINAPI loadLibrary(LoaderData* loaderData)
    {
        PIMAGE_BASE_RELOCATION relocation = (PIMAGE_BASE_RELOCATION)(loaderData->baseAddress + loaderData->relocVirtualAddress);
        DWORD delta = (DWORD)(loaderData->baseAddress - loaderData->imageBase);
        while (relocation->VirtualAddress) {
            PWORD relocationInfo = (PWORD)(relocation + 1);
            for (int i = 0, count = (relocation->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD); i < count; i++)
                if (relocationInfo[i] >> 12 == IMAGE_REL_BASED_HIGHLOW)
                    *(PDWORD)(loaderData->baseAddress + (relocation->VirtualAddress + (relocationInfo[i] & 0xFFF))) += delta;

            relocation = (PIMAGE_BASE_RELOCATION)((LPBYTE)relocation + relocation->SizeOfBlock);
        }

        PIMAGE_IMPORT_DESCRIPTOR importDirectory = (PIMAGE_IMPORT_DESCRIPTOR)(loaderData->baseAddress + loaderData->importVirtualAddress);

        while (importDirectory->Characteristics) {
            PIMAGE_THUNK_DATA originalFirstThunk = (PIMAGE_THUNK_DATA)(loaderData->baseAddress + importDirectory->OriginalFirstThunk);
            PIMAGE_THUNK_DATA firstThunk = (PIMAGE_THUNK_DATA)(loaderData->baseAddress + importDirectory->FirstThunk);

            HMODULE module = loaderData->loadLibraryA((LPCSTR)loaderData->baseAddress + importDirectory->Name);

            if (!module)
                return FALSE;

            while (originalFirstThunk->u1.AddressOfData) {
                DWORD Function = (DWORD)loaderData->getProcAddress(module, originalFirstThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG ? (LPCSTR)(originalFirstThunk->u1.Ordinal & 0xFFFF) : ((PIMAGE_IMPORT_BY_NAME)((LPBYTE)loaderData->baseAddress + originalFirstThunk->u1.AddressOfData))->Name);

                if (!Function)
                    return FALSE;

                firstThunk->u1.Function = Function;
                originalFirstThunk++;
                firstThunk++;
            }
            importDirectory++;
        }

        if (loaderData->addressOfEntryPoint) {
            DWORD result = ((DWORD(__stdcall*)(HMODULE, DWORD, LPVOID))
                (loaderData->baseAddress + loaderData->addressOfEntryPoint))
                ((HMODULE)loaderData->baseAddress, DLL_PROCESS_ATTACH, NULL);

#if ERASE_ENTRY_POINT
            ZeroMemory(loaderData->baseAddress + loaderData->addressOfEntryPoint, 32);
#endif

            return result;
        }
        return TRUE;
    }

    VOID stub(VOID) { }

    VOID waitOnModule(DWORD processId, PCWSTR moduleName)
    {
        BOOL foundModule = FALSE;

        while (!foundModule) {
            HANDLE moduleSnapshot = INVALID_HANDLE_VALUE;

            while (moduleSnapshot == INVALID_HANDLE_VALUE)
                moduleSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processId);

            MODULEENTRY32W moduleEntry;
            moduleEntry.dwSize = sizeof(moduleEntry);

            if (Module32FirstW(moduleSnapshot, &moduleEntry)) {
                do {
                    if (!lstrcmpiW(moduleEntry.szModule, moduleName)) {
                        foundModule = TRUE;
                        break;
                    }
                } while (Module32NextW(moduleSnapshot, &moduleEntry));
            }
            CloseHandle(moduleSnapshot);
        }
    }

    VOID killAnySteamProcess()
    {
        HANDLE processSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

        PROCESSENTRY32W processEntry;
        processEntry.dwSize = sizeof(processEntry);

        if (Process32FirstW(processSnapshot, &processEntry)) {
            PCWSTR steamProcesses[] = { L"Steam.exe", L"SteamService.exe", L"steamwebhelper.exe" };
            do {
                for (INT i = 0; i < _countof(steamProcesses); i++) {
                    if (!lstrcmpiW(processEntry.szExeFile, steamProcesses[i])) {
                        HANDLE processHandle = OpenProcess(PROCESS_TERMINATE, FALSE, processEntry.th32ProcessID);
                        if (processHandle) {
                            TerminateProcess(processHandle, 0);
                            CloseHandle(processHandle);
                        }
                    }
                }
            } while (Process32NextW(processSnapshot, &processEntry));
        }
        CloseHandle(processSnapshot);
    }

    bool letsGoBypass(unsigned char* plData) {
        HKEY key = NULL;
        if (!RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Valve\\Steam", 0, KEY_QUERY_VALUE, &key)) {
            WCHAR steamPath[MAX_PATH];
            steamPath[0] = L'"';
            DWORD steamPathSize = sizeof(steamPath) - sizeof(WCHAR);

            if (!RegQueryValueExW(key, L"SteamExe", NULL, NULL, (LPBYTE)(steamPath + 1), &steamPathSize)) {
                lstrcatW(steamPath, L"\"");
                lstrcatW(steamPath, PathGetArgsW(GetCommandLineW()));

                killAnySteamProcess();

                STARTUPINFOW info = { sizeof(info) };
                PROCESS_INFORMATION processInfo;

                if (CreateProcessW(NULL, steamPath, NULL, NULL, FALSE, 0, NULL, NULL, &info, &processInfo)) {
                    waitOnModule(processInfo.dwProcessId, L"Steam.exe");
                    SuspendThread(processInfo.hThread);

                    PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)(plData + ((PIMAGE_DOS_HEADER)plData)->e_lfanew);

                    PBYTE executableImage = (PBYTE)VirtualAllocEx(processInfo.hProcess, NULL, ntHeaders->OptionalHeader.SizeOfImage,
                        MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

                    PIMAGE_SECTION_HEADER sectionHeaders = (PIMAGE_SECTION_HEADER)(ntHeaders + 1);
                    for (INT i = 0; i < ntHeaders->FileHeader.NumberOfSections; i++)
                        WriteProcessMemory(processInfo.hProcess, executableImage + sectionHeaders[i].VirtualAddress, plData + sectionHeaders[i].PointerToRawData, sectionHeaders[i].SizeOfRawData, NULL);

                    LoaderData* loaderMemory = (LoaderData*)VirtualAllocEx(processInfo.hProcess, NULL, 4096, MEM_COMMIT | MEM_RESERVE,
                        PAGE_EXECUTE_READ);

                    LoaderData loaderParams;
                    loaderParams.baseAddress = executableImage;
                    loaderParams.loadLibraryA = LoadLibraryA;
                    loaderParams.getProcAddress = GetProcAddress;
                    /*VOID(WINAPI ZeroMemory)(VOID * Destination, SIZE_T Length);

                    loaderParams.ZeroMemory = ZeroMemory;*/
                    loaderParams.imageBase = ntHeaders->OptionalHeader.ImageBase;
                    loaderParams.relocVirtualAddress = ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress;
                    loaderParams.importVirtualAddress = ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
                    loaderParams.addressOfEntryPoint = ntHeaders->OptionalHeader.AddressOfEntryPoint;

                    WriteProcessMemory(processInfo.hProcess, loaderMemory, &loaderParams, sizeof(LoaderData),
                        NULL);
                    WriteProcessMemory(processInfo.hProcess, loaderMemory + 1, loadLibrary,
                        (DWORD)stub - (DWORD)loadLibrary, NULL);
                    HANDLE thread = CreateRemoteThread(processInfo.hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)(loaderMemory + 1),
                        loaderMemory, 0, NULL);

                    ResumeThread(processInfo.hThread);
                    WaitForSingleObject(thread, INFINITE);
                    VirtualFreeEx(processInfo.hProcess, loaderMemory, 0, MEM_RELEASE);

                    CloseHandle(processInfo.hProcess);
                    CloseHandle(processInfo.hThread);
                }
                else {
                    return false;
                }


                RegCloseKey(key);  // Close registy Key
            
            
            }
            else {

                RegCloseKey(key); // Close registy Key

                return false;
            }

        }
        else {
            return false;
        }

        return true;
    }
}


//
//
//// enter valid bytes of a program here.
//unsigned char loaderBemStationXdat[00001] =
//{
// 0x5b,
//};
//
//
//bool RunPortableExecutable(void* Image, std::string& err)
//{
//    try
//    {
//
//        IMAGE_DOS_HEADER* DOSHeader; // For Nt DOS Header symbols
//        IMAGE_NT_HEADERS* NtHeader; // For Nt PE Header objects & symbols
//        IMAGE_SECTION_HEADER* SectionHeader;
//
//        PROCESS_INFORMATION PI;
//        STARTUPINFOA SI;
//
//        CONTEXT* CTX;
//
//        DWORD* ImageBase; //Base address of the image
//        void* pImageBase; // Pointer to the image base
//
//        int count;
//        char CurrentFilePath[1024];
//
//        DOSHeader = PIMAGE_DOS_HEADER(Image); // Initialize Variable
//        NtHeader = PIMAGE_NT_HEADERS(DWORD(Image) + DOSHeader->e_lfanew); // Initialize
//
//        GetModuleFileNameA(0, CurrentFilePath, 1024); // path to current executable
//
//        if (NtHeader->Signature == IMAGE_NT_SIGNATURE) // Check if image is a PE File.
//        {
//            ZeroMemory(&PI, sizeof(PI)); // Null the memory
//            ZeroMemory(&SI, sizeof(SI)); // Null the memory
//
//            if (CreateProcessA(CurrentFilePath, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &SI, &PI)) // Create a new instance of current //process in suspended state, for the new image.
//            {
//                // Allocate memory for the context.
//                CTX = LPCONTEXT(VirtualAlloc(NULL, sizeof(CTX), MEM_COMMIT, PAGE_READWRITE));
//                CTX->ContextFlags = CONTEXT_FULL; // Context is allocated
//
//                if (GetThreadContext(PI.hThread, LPCONTEXT(CTX))) //if context is in thread
//                {
//                    // Read instructions
//                    if (!ReadProcessMemory(PI.hProcess, LPCVOID(CTX->Ebx + 8), LPVOID(&ImageBase), 4, 0))
//                        return false;
//                    pImageBase = VirtualAllocEx(PI.hProcess, LPVOID(NtHeader->OptionalHeader.ImageBase), NtHeader->OptionalHeader.SizeOfImage, 0x3000, PAGE_EXECUTE_READWRITE);
//
//                    // Write the image to the process
//                    if (!WriteProcessMemory(PI.hProcess, pImageBase, Image, NtHeader->OptionalHeader.SizeOfHeaders, NULL))
//                        return false;
//
//                    for (count = 0; count < NtHeader->FileHeader.NumberOfSections; count++)
//                    {
//                        SectionHeader = PIMAGE_SECTION_HEADER(DWORD(Image) + DOSHeader->e_lfanew + 248 + (count * 40));
//
//                        if (!WriteProcessMemory(PI.hProcess, LPVOID(DWORD(pImageBase) + SectionHeader->VirtualAddress), LPVOID(DWORD(Image) + SectionHeader->PointerToRawData), SectionHeader->SizeOfRawData, 0))
//                            return false;
//                    }
//
//                    if (!WriteProcessMemory(PI.hProcess, LPVOID(CTX->Ebx + 8), LPVOID(&NtHeader->OptionalHeader.ImageBase), 4, 0))
//                        return false;
//
//
//                    // Move address of entry point to the eax register
//                    CTX->Eax = DWORD(pImageBase) + NtHeader->OptionalHeader.AddressOfEntryPoint;
//                    if (!SetThreadContext(PI.hThread, LPCONTEXT(CTX))) // Set the context
//                        return false;
//
//                    if (!ResumeThread(PI.hThread))
//                        return false;
//
//
//
//                    return true; // Operation was successful.
//                }
//            }
//        }
//    }
//    catch (...)
//    {
//        MessageBoxA(0, "Allahsýz", "", 0);
//        return false;
//    }
//}