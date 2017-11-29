#include <windows.h>
#include "self_dll_info.h"

#include "output_debugstream.h"


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:

		//-------------------------------------------------------------------------
		// 自分自身のdllのハンドルやフルパスを保存しておく
		//-------------------------------------------------------------------------
		CSelfDllInfo::InitializeHandle(hModule);

		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}



class DllLoadAndFree {
public:
	DllLoadAndFree() {
		OutputDebugStream(L"hmPython3 Load");
	}
	~DllLoadAndFree() {
		OutputDebugStream(L"hmPython3 Free");
	}
};

static DllLoadAndFree dlaf;