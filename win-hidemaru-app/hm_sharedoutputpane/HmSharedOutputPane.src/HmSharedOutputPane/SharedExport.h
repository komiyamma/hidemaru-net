#pragma once


#include <windows.h>
#include <tchar.h>
#include <string>


#define BUF_LINE 5000
#define BUF_CHRS 1024

namespace std {
	using tstring = std::basic_string<TCHAR>;
}

using PFNOUTPUT = int(_cdecl *)(HWND hwnd, const TCHAR *);
using PFNGETWINDOWHANDLE = HWND(_cdecl *)(HWND hHidemaru);
extern HWND hCurHidemaruWndHandle;
extern "C" void _stdcall SetSharedMessage(const TCHAR *szmsg);

unsigned __stdcall OutputSharedMessage(void *);

extern TCHAR szBufList[BUF_LINE][BUF_CHRS];

extern HMODULE hHmOutputPaneDLL;

extern PFNOUTPUT pOutputFunc;
extern PFNGETWINDOWHANDLE pOutputGetWindowFunc;