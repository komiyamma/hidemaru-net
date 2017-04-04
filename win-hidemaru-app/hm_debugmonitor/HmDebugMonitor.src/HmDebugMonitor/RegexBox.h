#pragma once

#include <windows.h>
#include <richedit.h>

const int RegexBoxWidth = 150;

extern HWND hRgxEdit;

extern char hRgxEditContent[4096]; // 正規表現領域からテキストを抽出

HWND CreateRegexBox(HWND hWnd, HINSTANCE hInst);
void DeleteRegexBox();
BOOL SetInitialFont(HWND hWnd);
void MoveRegexBox(HWND hWnd);