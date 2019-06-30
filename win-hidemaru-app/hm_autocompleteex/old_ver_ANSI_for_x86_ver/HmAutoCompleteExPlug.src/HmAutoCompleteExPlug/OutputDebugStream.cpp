#include <windows.h>
#include <stdio.h>

using namespace System;

#pragma unmanaged
void OutputDebugStream(const char *format, ...)
{
	char szBufDebug[4096] = "";

	va_list arg;

	va_start(arg, format);
	vsprintf_s(szBufDebug, format, arg);
	va_end(arg);

	strcat_s(szBufDebug, "\n");
	OutputDebugString(szBufDebug);
}


#pragma managed
void OutputDebugStream(String^ message)
{
	char szBufDebug[4096] = "";
	wsprintf(szBufDebug, "%s", message);
	OutputDebugString(szBufDebug);
}
