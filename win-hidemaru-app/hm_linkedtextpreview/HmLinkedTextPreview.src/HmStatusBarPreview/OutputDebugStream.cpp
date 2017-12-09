/*
 * Copyright(C) 2017 Akitsugu Komiyama
 * under the MIT License
 */ 

#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <string>


#pragma unmanaged
void OutputDebugStream(const wchar_t *format, ...) {
	wchar_t szBufDebug[4096] = _T("");

	va_list arg;

	va_start(arg, format);
	_vstprintf_s(szBufDebug, format, arg);
	va_end(arg);

	_tcscat(szBufDebug, _T("\n"));
	OutputDebugString(szBufDebug);
}
#pragma managed

