/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <tchar.h>


void _TheOutputDebugStream(const TCHAR *format, ...) {
	TCHAR szBufDebug[4096] = _T("");

	va_list arg;

	va_start(arg, format);
	_vstprintf_s(szBufDebug, format, arg);
	va_end(arg);

	_tcscat(szBufDebug, _T("\n"));
	OutputDebugString(szBufDebug);
}
