#pragma once

#include <windows.h>
#include <string>

using namespace std;

class CHidemaruExeExport {
public:
	CHidemaruExeExport();
private:
	static HMODULE hHideExeHandle;
	static TCHAR szHidemaruFullPath[MAX_PATH];

private:
	//========================================================================
	// 秀丸本体のバージョン。
	//-------------------------------------------------------------------------
	// 8.66β3なら、「866.03」みたいな感じになる。
	//========================================================================
	static double hm_version;
	static double QueryFileVersion(TCHAR* path);

protected:
	//========================================================================
	/// 秀丸本体からExportされているいくつかの関数群
	//========================================================================

	//-------------------------------------------------------------------------
	// 現在編集中の全てのテキストを得る
	using PFNGetTotalTextUnicode = HGLOBAL(WINAPI *)(void);
	static PFNGetTotalTextUnicode Hidemaru_GetTotalTextUnicode;

	using PFNGetCurrentWindowHandle = HWND(WINAPI *)(void);
	static PFNGetCurrentWindowHandle Hidemaru_GetCurrentWindowHandle;

public:


	//========================================================================
	/// ラップ関数郡
	//========================================================================

	//-------------------------------------------------------------------------
	// Hidemaru_GetTotalTextUnicode関数のラップしたもの
	//-------------------------------------------------------------------------
	static wstring GetTotalText();


};


