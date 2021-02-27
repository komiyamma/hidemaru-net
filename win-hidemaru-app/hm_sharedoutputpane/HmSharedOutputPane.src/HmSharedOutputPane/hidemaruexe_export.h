/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#pragma once

#include <windows.h>
#include <string>

using namespace std;

class CHidemaruExeExport {
public:
	CHidemaruExeExport();
private:
	static HMODULE hHideExeHandle;
	static wchar_t szHidemaruFullPath[MAX_PATH];


protected:
	//========================================================================
	/// �G�ۖ{�̂���Export����Ă��邢�����̊֐��Q
	//========================================================================


	using PFNGetCurrentWindowHandle = HWND(WINAPI *)(void);
	static PFNGetCurrentWindowHandle Hidemaru_GetCurrentWindowHandle;

public:


private:
	static HWND hCurWndHidemaru;
public:
	static HWND GetCurWndHidemaru();

};


