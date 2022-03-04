// dllmain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
#include "pch.h"
#include "resource.h"
#include "windows.h"
#include <memory>

// 秀丸のウィンドウハンドル
using PFNGetCurrentWindowHandle = HWND(WINAPI*)();
PFNGetCurrentWindowHandle Hidemaru_GetCurrentWindowHandle = NULL;

wchar_t wszHidemaruFullPath[MAX_PATH] = L"";
HMODULE hHideExeHandle = NULL;
HWND hCurrentWindowHandle = NULL;
HWND GetCurrentHidemaruWindowHandle() {
	if (hCurrentWindowHandle) {
		return hCurrentWindowHandle;
	}
	if (!hHideExeHandle) {
		// 秀丸本体に関数があるのは 8.73.99以上
		GetModuleFileName(NULL, wszHidemaruFullPath, _countof(wszHidemaruFullPath));
		hHideExeHandle = LoadLibrary(wszHidemaruFullPath);
		if (hHideExeHandle) {
			Hidemaru_GetCurrentWindowHandle = (PFNGetCurrentWindowHandle)GetProcAddress(hHideExeHandle, "Hidemaru_GetCurrentWindowHandle");
		}
	}
	if (Hidemaru_GetCurrentWindowHandle) {
		hCurrentWindowHandle = Hidemaru_GetCurrentWindowHandle();
		return hCurrentWindowHandle;
	}

	return NULL;
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH: {
		HICON hIcon = LoadIcon(hModule, MAKEINTRESOURCE(IDI_ICON1));

		HWND hwnd = FindWindow(L"Hidemaru32Class", NULL);
		if (hwnd) {
			//Change both icons to the same icon handle.
			SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
			SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

			//This will ensure that the application icon gets changed too.
			SendMessage(GetWindow(hwnd, GW_OWNER), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
			SendMessage(GetWindow(hwnd, GW_OWNER), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
		}

		break;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

