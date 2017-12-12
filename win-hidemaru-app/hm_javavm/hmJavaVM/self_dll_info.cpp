/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#include <windows.h>
#include <shlwapi.h>

#include "self_dll_info.h"
#include "hidemaruexe_export.h"

HMODULE CSelfDllInfo::hModule = NULL;

TCHAR CSelfDllInfo::szSelfModuleFullPath[MAX_PATH] = L"";
TCHAR CSelfDllInfo::szSelfModuleDirPath[MAX_PATH] = L"";

int CSelfDllInfo::iSelfBindedType = 0;

void CSelfDllInfo::InitializeHandle(HMODULE hModule) {
	CSelfDllInfo::hModule = hModule;
	GetModuleFileName(hModule, CSelfDllInfo::szSelfModuleFullPath, _countof(CSelfDllInfo::szSelfModuleFullPath));
	wcscpy_s(CSelfDllInfo::szSelfModuleDirPath, CSelfDllInfo::szSelfModuleFullPath);
	PathRemoveFileSpec(CSelfDllInfo::szSelfModuleDirPath);
}

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
