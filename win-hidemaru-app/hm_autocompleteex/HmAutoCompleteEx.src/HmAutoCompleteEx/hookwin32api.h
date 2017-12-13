/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#pragma once

#include <windows.h>

extern void HookWin32apiFuncs();
extern PROC hook_win32api(char *szModuleName, char *szFuncName, void *szFuncAddress);

// �t�b�N����֐��̃v���g�^�C�v���`
using PFNSENDMESSAGEA = LRESULT(WINAPI *)(HWND, UINT, WPARAM, LPARAM);

extern PROC pfnPrevSendMessageA; // �P�O��SendMessage�֐��B
extern PROC pfnPrevSendMessageW; // �P�O��SendMessage�֐��B
