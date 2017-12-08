/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the MIT License
 */

#include <windows.h>
#include <tchar.h>
#include <sstream>

#include "string_converter.h"

using namespace std;
using namespace System;

void OutputDebugStream(wstring wstr) {
	System::Diagnostics::Trace::WriteLine(wstring_to_String(wstr));
}


