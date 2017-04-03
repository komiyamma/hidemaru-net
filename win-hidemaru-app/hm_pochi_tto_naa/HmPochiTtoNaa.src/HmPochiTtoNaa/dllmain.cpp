#include <string>
#include "dllmain.h"

using namespace std;

#pragma comment(lib, "shlwapi.lib")

static TCHAR szMySelfFileName[MAX_PATH];
wstring GetSelfDllFullPath() {
	return szMySelfFileName;
}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved ) {

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:

		// é©êgÇÃñºëOÇÃäiî[
		GetModuleFileName((HMODULE)hModule, szMySelfFileName, _countof(szMySelfFileName));

		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:

		break;

	default:
		break;
	}
	return TRUE;
}

