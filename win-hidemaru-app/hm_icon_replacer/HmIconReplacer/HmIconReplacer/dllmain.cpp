// dllmain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
#include <windows.h>
#include "resource.h"
#include <memory>
#include <string>
#include "dllmain.h"

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

void ChangeHidemaruIcon(const HICON hIcon)
{
	HWND hwnd = FindWindow(L"Hidemaru32Class", NULL);
	if (hwnd) {
		//Change both icons to the same icon handle.
		SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
		SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

		//This will ensure that the application icon gets changed too.
		SendMessage(GetWindow(hwnd, GW_OWNER), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
		SendMessage(GetWindow(hwnd, GW_OWNER), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	}
}

HMODULE dllHandle = NULL;
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH: {
		dllHandle = hModule;
		break;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}


extern "C" __declspec(dllexport) BOOL ChangeIcon(wchar_t *icon_filepath)
{
	if (dllHandle) {

		HICON hIcon = (HICON)LoadImage(NULL, icon_filepath, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
		/*
		if (!hIcon) {
			hIcon = LoadIcon(dllHandle, MAKEINTRESOURCE(IDI_ICON1));
		}
		*/
		if (hIcon) {
			ChangeHidemaruIcon(hIcon);
		}
	}

	return TRUE;
}

extern "C" __declspec(dllexport) BOOL ResetIcon()
{
	HMODULE hideHandle = GetModuleHandle(NULL);
	HICON hIcon = LoadIcon(hideHandle, MAKEINTRESOURCE(IDI_ICON1));
	ChangeHidemaruIcon(hIcon);

	return TRUE;
}

