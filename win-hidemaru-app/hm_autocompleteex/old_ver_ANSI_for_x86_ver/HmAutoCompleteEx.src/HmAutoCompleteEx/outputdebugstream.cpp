#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>


void _TheOutputDebugStream(const char *format, ...) {
	char szBufDebug[4096] = "";

	va_list arg;

	va_start(arg, format);
	vsprintf_s(szBufDebug, format, arg);
	va_end(arg);

	strcat(szBufDebug, "\n");
	OutputDebugString(szBufDebug);
}
