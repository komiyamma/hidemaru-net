#pragma once

#include <windows.h>

extern "C" PROC hook_win32api(char *szModuleName, char *szFuncName, void *szFuncAddress);

// �t�b�N����֐��̃v���g�^�C�v���`
using PFNSENDMESSAGEA = LRESULT(WINAPI *)(HWND, UINT, WPARAM, LPARAM);

extern PROC pfnPrevSendMessageA; // �P�O��SendMessage�֐��B
