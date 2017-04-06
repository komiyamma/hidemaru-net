#pragma once

#include <windows.h>

extern void HookWin32apiFuncs();
extern PROC hook_win32api(char *szModuleName, char *szFuncName, void *szFuncAddress);

// フックする関数のプロトタイプを定義
using PFNSENDMESSAGEA = LRESULT(WINAPI *)(HWND, UINT, WPARAM, LPARAM);

extern PROC pfnPrevSendMessageA; // １つ前のSendMessage関数。
extern PROC pfnPrevSendMessageW; // １つ前のSendMessage関数。
