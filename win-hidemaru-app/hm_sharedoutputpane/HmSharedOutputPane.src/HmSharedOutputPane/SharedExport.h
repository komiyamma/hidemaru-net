/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#pragma once


#include <windows.h>
#include <tchar.h>
#include <string>


#define BUF_LINE 5000
#define BUF_CHRS 1024



using PFNOUTPUT = int(_cdecl *)(HWND hwnd, BYTE *);
using PFNGETWINDOWHANDLE = HWND(_cdecl *)(HWND hHidemaru);
extern HWND hCurHidemaruWndHandle;
extern "C" void _stdcall SetSharedMessage(const char *szmsg);
extern "C" void _stdcall SetSharedMessageA(const char *szmsg);
extern "C" void _stdcall SetSharedMessageW(const wchar_t *szmsg);

unsigned __stdcall OutputSharedMessage(void *);

extern wchar_t szBufList[BUF_LINE][BUF_CHRS];

extern HMODULE hHmOutputPaneDLL;

extern PFNOUTPUT pOutputFunc;
extern PFNGETWINDOWHANDLE pOutputGetWindowFunc;