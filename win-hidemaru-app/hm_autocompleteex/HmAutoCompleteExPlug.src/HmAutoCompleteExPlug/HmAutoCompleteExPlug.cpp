/*
 * This codes is licensed under CC0 1.0 Universal
 */

#include <windows.h>
#include "OutputDebugStream.h"

#define MACRO_DLL extern "C" __declspec(dllexport)


MACRO_DLL int OnCreate(HWND hWnd, LPCTSTR pwszFileName) {
	OutputDebugStream(L"OnCreate:%s\n", pwszFileName);
	return TRUE;
}
 
MACRO_DLL int OnListBoxSelectedIndexChanged(HWND hWnd, int iListBoxSelectedIndex, LPCTSTR pwszListBoxSelectedItem, int iItemHeight) {
	OutputDebugStream(L"OnListBoxSelectedIndexChanged:%s\n", pwszListBoxSelectedItem);
	return TRUE;
}

MACRO_DLL int OnDestroy(HWND hWnd) {
	OutputDebugStream(L"OnDestroy\n");
	return TRUE;
}

using AUTOCOMPLETELIST = void;
MACRO_DLL AUTOCOMPLETELIST* OnQueryListBoxCustomAdded(HWND hWnd, AUTOCOMPLETELIST *pAutoCompleteOriginalList) {
	OutputDebugStream(L"OnQueryListBoxCustomAdded:\n");
	return NULL;
}

MACRO_DLL int OnKeyDown(HWND hWnd, WPARAM wParam) {
	OutputDebugStream(L"OnKeyDown:%c\n", wParam);
	return TRUE;
}
