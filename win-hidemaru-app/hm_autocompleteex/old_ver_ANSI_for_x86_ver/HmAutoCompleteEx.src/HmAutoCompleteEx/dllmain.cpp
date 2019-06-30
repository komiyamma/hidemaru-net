#include <windows.h>
#include <stdio.h>

#include "outputdebugstream.h"
#include "autocompletewindow.h"
#include "hookwin32api.h"
#include "outlinerwindow.h"
#include "programmemory.h"
#include "OnigRegex.h"

using namespace std;


HANDLE curProcess = NULL;
char szHidemaru[MAX_PATH*2];
HINSTANCE hInst = NULL;


#pragma unmanaged
BOOL WINAPI DllMain(HINSTANCE hinstDll, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason) {

	case DLL_PROCESS_ATTACH: // DLLがプロセスのアドレス空間にマッピングされた。
		{
			// 鬼車の初期化
			onig_init();
			hInst = hinstDll;
			curProcess = ::GetCurrentProcess();
			GetModuleFileName(NULL, szHidemaru, sizeof(szHidemaru));
		}
		break;

	case DLL_THREAD_ATTACH: // スレッドが作成されようとしている。
		break;

	case DLL_THREAD_DETACH: // スレッドが破棄されようとしている。
		break;

	case DLL_PROCESS_DETACH: // DLLのマッピングが解除されようとしている。

		// 鬼車の終了
		onig_end();

		break;

	}

	return TRUE;
}

