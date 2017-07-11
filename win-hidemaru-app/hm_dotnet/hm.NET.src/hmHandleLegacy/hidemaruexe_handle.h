#pragma once
#include <windows.h>
#include <string>

using namespace std;
using namespace System;
extern HWND GetCurWndHidemaru(HWND hCurWnd);

// ストアアプリ版も考慮して鋳型にしておく
public ref class HidemaruWindowHandleSearcher {

public:
	HidemaruWindowHandleSearcher(String^ strClassName);

private:
	// これはスタティックではない。デスクトップ版とアプリ版があるので
	String^ strHidemaruClassName;
	// 秀丸ハンドル
	HWND hCurWndHidemaru;

public:
	HWND SearchCurWndHidemaru();

public:
	// 外との接点
	static IntPtr GetCurWndHidemaru(IntPtr curWnd);


private:
	bool IsWndHidemaru32ClassType(HWND hWnd);
	// 秀丸が変則的なことになっていない（例えば、他のアプリの子ウィンドウとかに強引にparentWindowsを捻じ曲げられているなどといったことになっていない)
	void TabOnSearchCurWndHidemaru(HWND hWnd);
	void NoTabSearchCurWndHidemaru(HWND hWnd);

}; 
