/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#include <windows.h>
#include <tchar.h>

#include "hidemaruexe_export.h"
#include "hidemaruexe_handle.h"

#pragma comment(lib, "version.lib")


HWND CHidemaruExeExport::hCurWndHidemaru = NULL;
HMODULE CHidemaruExeExport::hHideExeHandle = NULL;
wchar_t CHidemaruExeExport::szHidemaruFullPath[MAX_PATH] = L"";

CHidemaruExeExport::PFNGetCurrentWindowHandle CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle = NULL;


CHidemaruExeExport::CHidemaruExeExport() {

	if (hHideExeHandle) {
		return;
	}

	GetModuleFileNameW(NULL, szHidemaruFullPath, _countof(szHidemaruFullPath));

	// èGä€ñ{ëÃÇ…ä÷êîÇ™Ç†ÇÈÇÃÇÕ 8.66à»è„
	hHideExeHandle = LoadLibraryW(szHidemaruFullPath);

	if (hHideExeHandle) {
		Hidemaru_GetCurrentWindowHandle = (PFNGetCurrentWindowHandle)GetProcAddress(hHideExeHandle, "Hidemaru_GetCurrentWindowHandle");
		return;
	}
}

HWND CHidemaruExeExport::GetCurWndHidemaru() {
	if (Hidemaru_GetCurrentWindowHandle) {
		hCurWndHidemaru = Hidemaru_GetCurrentWindowHandle();
	}
	else {
		hCurWndHidemaru = ::GetCurWndHidemaru(hCurWndHidemaru);
	}
	return hCurWndHidemaru;
}

