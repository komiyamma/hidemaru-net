/*
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the MIT License
 */

#pragma once

#include <windows.h>
#include <vector>

using namespace std;

void InitDebugMonitor(HWND hEdit); // 初期関数

// スレッド用関数の宣言
unsigned __stdcall ThreadExternalProcDebugMonitor(void *);

extern BOOL isDoingRecieveFlag;
extern int iHidemaruErrorCnt;