/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#include <windows.h>

using namespace std;


#pragma unmanaged
BOOL WINAPI DllMain(HINSTANCE hinstDll, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason) {

	case DLL_PROCESS_ATTACH: // DLLがプロセスのアドレス空間にマッピングされた。
		break;
	case DLL_THREAD_ATTACH: // スレッドが作成されようとしている。
		break;
	case DLL_THREAD_DETACH: // スレッドが破棄されようとしている。
		break;
	case DLL_PROCESS_DETACH: // DLLのマッピングが解除されようとしている。
		break;

	}

	return TRUE;
}

