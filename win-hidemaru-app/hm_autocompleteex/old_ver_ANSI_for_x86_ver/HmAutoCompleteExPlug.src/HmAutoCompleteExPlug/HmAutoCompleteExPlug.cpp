#include <windows.h>
#include "OutputDebugStream.h"

extern "C" __declspec(dllexport) int OnCreate(HWND hWnd, LPCTSTR szFileName) {
	OutputDebugStream("OnCreate:%s\n", szFileName );
	return TRUE;
}
 
extern "C" __declspec(dllexport) int OnListBoxSelectedIndexChanged(HWND hWnd, int iListBoxSelectedIndex, LPCTSTR szListBoxSelectedItem, int iItemHeight) {
	OutputDebugStream("OnListBoxSelectedIndexChanged:%s\n", szListBoxSelectedItem);
	return TRUE;
}

extern "C" __declspec(dllexport) int OnDestroy(HWND hWnd) {
	OutputDebugStream("OnDestroy\n");
	return TRUE;
}

using AUTOCOMPLETELIST = void;
extern "C" __declspec(dllexport) AUTOCOMPLETELIST* OnQueryListBoxCustomAdded(HWND hWnd, AUTOCOMPLETELIST *pAutoCompleteOriginalList) {
	OutputDebugStream("OnQueryListBoxCustomAdded:\n");
	return NULL;
}

extern "C" __declspec(dllexport) int OnKeyDown(HWND hWnd, WPARAM wParam) {
	OutputDebugStream("OnKeyDown:%c\n", wParam );
	return TRUE;
}
