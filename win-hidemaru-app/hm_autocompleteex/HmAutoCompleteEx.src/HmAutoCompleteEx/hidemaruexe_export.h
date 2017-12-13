/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#pragma once

#include <windows.h>
#include <string>

using namespace std;


#define WM_HIDEMARUINFO (WM_USER + 181)

#define HIDEMARUINFO_GETTABWIDTH 0
#define HIDEMARUINFO_GETSTARTCOLUMN 1
#define HIDEMARUINFO_GETSPACETAB 2
#define HIDEMARUINFO_GETMANUALTAB 3
#define HIDEMARUINFO_GETFILEFULLPATH 4


class CHidemaruExeExport {
private:
	static HMODULE hHideExeHandle;
	static TCHAR szHidemaruFullPath[MAX_PATH];

public:
	//========================================================================
	// 秀丸本体のバージョン。
	//-------------------------------------------------------------------------
	// 8.66β3なら、「866.03」みたいな感じになる。
	//========================================================================
	static double hm_version;
private:
	static double QueryFileVersion(TCHAR* path);

public:

	//========================================================================
	/// 初期化
	//========================================================================
	CHidemaruExeExport();
};


