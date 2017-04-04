#pragma once

#include <windows.h>
#include <string>
using namespace std;

namespace HmStatusBarText {
	void OnEnumChildProc(HWND hWnd);
	void ClearCache();
}
