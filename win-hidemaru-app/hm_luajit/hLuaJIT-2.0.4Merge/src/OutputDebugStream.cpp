/* 
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the MIT License
 */

#include "lua_interface.h"

void lua_OutputDebugStream(const char *format, ...) {
	char szBufDebug[4096] = "";
	int length = sizeof(szBufDebug);

	va_list arg;

	va_start(arg, format);
	vprintf(format, arg);
	_vsnprintf_s(szBufDebug, length, _TRUNCATE, format, arg);
	va_end(arg);

	szBufDebug[length - 1] = '\0'; // ”Ô•º

	OutputDebugString(szBufDebug);
}
