

namespace BypassLoader
{
    void LoaderLoop(HMODULE hModule);
};

namespace LibraryLoader
{
	bool inject(DWORD processName, char* dllPath);
}