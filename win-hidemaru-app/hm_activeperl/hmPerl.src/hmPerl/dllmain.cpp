/*
 Copyright (c) 2016-2017 Akitsugu Komiyama
 under the Apache License Version 2.0
*/

#include <windows.h>
#include "self_dll_info.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:

		//-------------------------------------------------------------------------
		// �������g��dll�̃n���h����t���p�X��ۑ����Ă���
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