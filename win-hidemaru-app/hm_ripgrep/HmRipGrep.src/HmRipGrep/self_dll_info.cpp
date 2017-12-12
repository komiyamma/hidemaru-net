/*
 * Copyright (C) 2017 Akitsugu Komiyama
 * under the MIT License
 */ 

#include <windows.h>

#include "self_dll_info.h"
#include "hidemaruexe_export.h"

HMODULE CSelfDllInfo::hModule = NULL;

TCHAR CSelfDllInfo::szSelfModuleFullPath[MAX_PATH] = L"";
TCHAR CSelfDllInfo::szSelfModuleDirPath[MAX_PATH] = L"";

int CSelfDllInfo::iSelfBindedType = 0;

int CSelfDllInfo::GetBindDllType() {
	return iSelfBindedType;
}

BOOL CSelfDllInfo::SetBindDllHandle() {
	// 本体からの関数の生成が先
	CHidemaruExeExport::init();

	// 秀丸8.66以上
	if (CHidemaruExeExport::Hidemaru_GetDllFuncCalledType) {
		int dll = CHidemaruExeExport::Hidemaru_GetDllFuncCalledType(-1); // 自分のdllの呼ばれ方をチェック
		CSelfDllInfo::iSelfBindedType = dll;
		return TRUE;
	}
	else {
		MessageBox(NULL, L"loadllのパターンが認識出来ませんでした。", L"loadllのパターンが認識出来ませんでした。", MB_ICONERROR);
	}
	return FALSE;

}

wstring CSelfDllInfo::GetInvocantString() {
	if (iSelfBindedType == -1) {
		return L"";
	}
	else {
		return to_wstring(iSelfBindedType) + L",";
	}
}


wstring CSelfDllInfo::GetSelfModuleFullPath() {
	return szSelfModuleFullPath;
}

wstring CSelfDllInfo::GetSelfModuleDir() {
	return szSelfModuleDirPath;
}
