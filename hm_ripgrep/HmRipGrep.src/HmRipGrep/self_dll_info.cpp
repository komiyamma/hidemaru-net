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
	// �{�̂���̊֐��̐�������
	CHidemaruExeExport::init();

	// �G��8.66�ȏ�
	if (CHidemaruExeExport::Hidemaru_GetDllFuncCalledType) {
		int dll = CHidemaruExeExport::Hidemaru_GetDllFuncCalledType(-1); // ������dll�̌Ă΂�����`�F�b�N
		CSelfDllInfo::iSelfBindedType = dll;
		return TRUE;
	}
	else {
		MessageBox(NULL, L"loadll�̃p�^�[�����F���o���܂���ł����B", L"loadll�̃p�^�[�����F���o���܂���ł����B", MB_ICONERROR);
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
