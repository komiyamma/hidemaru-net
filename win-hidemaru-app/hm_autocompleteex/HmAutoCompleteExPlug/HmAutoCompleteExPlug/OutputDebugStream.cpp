#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <string>

using namespace std;

using namespace System;

void OutputDebugStream(const TCHAR *format, ...);
{
	TCHAR data[4096];
	va_list ap;

	va_start(ap, format);
	_vstprintf(data, format, ap);
	va_end(ap);

	OutputDebugString(data);
	OutputDebugString(L"\n");
}


void OutputDebugStream(String^ message)
{
	System::Diagnostics::Trace::WriteLine(message);
}
