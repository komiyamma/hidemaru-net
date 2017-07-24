#pragma once
#include <windows.h>
#include <string>

using namespace std;

extern HWND GetCurWndHidemaru(HWND hCurWnd);

// ストアアプリ版も考慮して鋳型にしておく
class HidemaruWindowHandleSearcher {

public:
	HidemaruWindowHandleSearcher(wstring strClassName);

private:
	// これはスタティックではない。デスクトップ版とアプリ版があるので
	wstring strHidemaruClassName;
	// 秀丸ハンドル
	HWND hCurWndHidemaru;

public:
	HWND GetCurWndHidemaru();


private:
	bool IsWndHidemaru32ClassType(HWND hWnd);
	// 秀丸が変則的なことになっていない（例えば、他のアプリの子ウィンドウとかに強引にparentWindowsを捻じ曲げられているなどといったことになっていない)
	void TabOnSearchCurWndHidemaru(HWND hWnd);
	void NoTabSearchCurWndHidemaru(HWND hWnd);

};
