/*
 * Copyright (C) 2017 Akitsugu Komiyama
 * under the MIT License
 */ 

#include <windows.h>
#include <shlwapi.h>
#include "self_dll_info.h"
#include "hidemaruexe_export.h"
#pragma comment(lib, "shlwapi.lib")

#pragma unmanaged
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:

		//-------------------------------------------------------------------------
		// �������g��dll�̃n���h����t���p�X��ۑ����Ă���
		//-------------------------------------------------------------------------
		CSelfDllInfo::hModule = hModule;
		GetModuleFileName(hModule, CSelfDllInfo::szSelfModuleFullPath, _countof(CSelfDllInfo::szSelfModuleFullPath));
		_tcscpy_s(CSelfDllInfo::szSelfModuleDirPath, CSelfDllInfo::szSelfModuleFullPath);
		PathRemoveFileSpec(CSelfDllInfo::szSelfModuleDirPath);
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
#pragma managed