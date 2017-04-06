#pragma once

#include <tchar.h>

using namespace System;

void OutputDebugStream(const TCHAR *format, ...);
void OutputDebugStream(String^ message);