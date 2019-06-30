/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <shlwapi.h>
#include "tstring.h"

#pragma comment(lib, "shlwapi.lib")

#include "outputdebugstream.h"
#include "autocompletewindow.h"
#include "hookwin32api.h"
#include "outlinerwindow.h"
#include "sethidemarufile.h"
#include "hidemaruexe_export.h"

using namespace std;


static HWND hWndHidemaru = NULL;
static TCHAR szCurrentFileFullPath[MAX_PATH * 2] = _T(""); // 秀丸のSetHidemaruHandleが呼ばれた時に最初にスロットに入るファイル名。途中でファイル名が変わることがあるので、あくまでもなかった場合用途

void SetCurrentFileName(HWND hWndHidemaru) {

	// 秀丸8.66以上であれば、ちゃんと現在進行形でファイル名を追い続ける
	if (CHidemaruExeExport::hm_version > 8.66 && hWndHidemaru) {

		// 現在の秀丸ウィンドウのファイル名を得る。
		LRESULT cwch = SendMessage(hWndHidemaru, WM_HIDEMARUINFO, HIDEMARUINFO_GETFILEFULLPATH, (LPARAM)szCurrentFileFullPath);
	}
}

TCHAR *GetCurrentFileName() {
	SetCurrentFileName(hWndHidemaru);
	return szCurrentFileFullPath;
}

intptr_t SetHidemaruHandle(HWND pWndHidemaru, TCHAR *szCurFileName) {
	hWndHidemaru = pWndHidemaru;
	HookWin32apiFuncs();

	OutputDebugStream(_T("■変わった瞬間のファイル名%s\n"), szCurFileName);

	// 2番目のパラメータとしてファイル名が来てるなら、代入する。8.66以前用
	if (szCurFileName) {
		_tcscpy(szCurrentFileFullPath, szCurFileName);
	}
	SetCurrentFileName(hWndHidemaru);

	return TRUE;
}

