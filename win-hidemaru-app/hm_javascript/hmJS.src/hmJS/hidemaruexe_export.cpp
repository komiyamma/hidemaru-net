/*
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#include <windows.h>
#include "hidemaruexe_export.h"


PFNGetDllFuncCalledType Hidemaru_GetDllFuncCalledType = NULL;

struct CHidemaruExeExporter {
	CHidemaruExeExporter() {
		// �G�� 8.66�ȏ�
		HMODULE hHideExe = GetModuleHandle(NULL);
		if (hHideExe) {
			Hidemaru_GetDllFuncCalledType = (PFNGetDllFuncCalledType)GetProcAddress(hHideExe, "Hidemaru_GetDllFuncCalledType");
		}
	}
};

CHidemaruExeExporter init;