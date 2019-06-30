/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#include <windows.h>

#include "SharedExport.h"
#include <process.h>    /* _beginthread, _endthread */

HANDLE thHandle = NULL;
extern bool isMustBreakLoop;

// 秀丸が自動でDllへのFreeLibraryの直前に呼び出す
extern "C" __declspec(dllexport) intptr_t DllDetachFunc_After_Hm866(){
	if (thHandle) {
		isMustBreakLoop = true;  // 終わるが良い
		WaitForSingleObject(thHandle, 70); // 遅くとも40ミリもあれば終わる予定
		isMustBreakLoop = false; // 終わったのでフラグだけ元へと戻す
		CloseHandle(thHandle);
		thHandle = NULL;
	}

	return TRUE;
}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:

		// アウトプットパネルのDLLのポインタを確保
		hHmOutputPaneDLL = LoadLibrary(_T("HmOutputPane.dll"));
		if (hHmOutputPaneDLL) {
			// アウトプットペインのOutput関数の取得
			// アウトプットペイン自体のウィンドウハンドルの取得。
			pOutputFunc = (PFNOUTPUT)GetProcAddress(hHmOutputPaneDLL, "Output");
			pOutputGetWindowFunc = (PFNGETWINDOWHANDLE)GetProcAddress(hHmOutputPaneDLL, "GetWindowHandle");

			// スレッドのスタート
			thHandle = (HANDLE)_beginthreadex(NULL, 0, OutputSharedMessage, 0, 0, NULL);
		}

		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	default:
		DllDetachFunc_After_Hm866();
		break;
	}
	return TRUE;
}

