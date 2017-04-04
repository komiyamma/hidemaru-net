#pragma once

#include <windows.h>
#include <vector>

using namespace std;

void InitDebugMonitor(HWND hEdit); // 初期関数

// スレッド用関数の宣言
unsigned __stdcall ThreadExternalProcDebugMonitor(void *);

extern BOOL isDoingRecieveFlag;
extern int iHidemaruErrorCnt;