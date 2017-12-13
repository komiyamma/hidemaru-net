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
	// �G�ۖ{�̂̃o�[�W�����B
	//-------------------------------------------------------------------------
	// 8.66��3�Ȃ�A�u866.03�v�݂����Ȋ����ɂȂ�B
	//========================================================================
	static double hm_version;
private:
	static double QueryFileVersion(TCHAR* path);

public:

	//========================================================================
	/// ������
	//========================================================================
	CHidemaruExeExport();
};


