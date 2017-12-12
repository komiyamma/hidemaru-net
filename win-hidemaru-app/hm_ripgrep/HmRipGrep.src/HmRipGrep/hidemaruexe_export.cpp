/*
 * Copyright (C) 2017 Akitsugu Komiyama
 * under the MIT License
 */ 

#include <windows.h>
#include <tchar.h>

#include "hidemaruexe_export.h"
#include "hidemaruexe_handle.h"
#include "output_debugstream.h"
#include "dllfunc_interface_internal.h"
#include "self_dll_info.h"

#pragma comment(lib, "version.lib")

HWND CHidemaruExeExport::hCurWndHidemaru = NULL;
HMODULE CHidemaruExeExport::hHideExeHandle = NULL;
TCHAR CHidemaruExeExport::szHidemaruFullPath[MAX_PATH] = L"";

CHidemaruExeExport::PFNGetCurrentWindowHandle CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle = NULL;
CHidemaruExeExport::PFNCheckQueueStatus CHidemaruExeExport::Hidemaru_CheckQueueStatus = NULL;
CHidemaruExeExport::PFNGetDllFuncCalledType CHidemaruExeExport::Hidemaru_GetDllFuncCalledType = NULL;
CHidemaruExeExport::PFNGetTotalTextUnicode CHidemaruExeExport::Hidemaru_GetTotalTextUnicode = NULL;
CHidemaruExeExport::PFNGetSelectedTextUnicode CHidemaruExeExport::Hidemaru_GetSelectedTextUnicode = NULL;
CHidemaruExeExport::PFNGetLineTextUnicode CHidemaruExeExport::Hidemaru_GetLineTextUnicode = NULL;
CHidemaruExeExport::PFNGetCursorPosUnicode CHidemaruExeExport::Hidemaru_GetCursorPosUnicode = NULL;
CHidemaruExeExport::PFNGetCursorPosUnicodeFromMousePos CHidemaruExeExport::Hidemaru_GetCursorPosUnicodeFromMousePos = NULL;
CHidemaruExeExport::PFNEvalMacro CHidemaruExeExport::Hidemaru_EvalMacro = NULL;

double CHidemaruExeExport::hm_version = 0;
double CHidemaruExeExport::QueryFileVersion(TCHAR* path){
	VS_FIXEDFILEINFO* v;
	DWORD dwZero = 0;
	UINT len;
	DWORD sz = GetFileVersionInfoSize(path, &dwZero);
	if (sz){
		void* buf = new char[sz];
		GetFileVersionInfo(path, dwZero, sz, buf);

		if (VerQueryValue(buf, L"\\", (LPVOID*)&v, &len)){
			double ret = 0;
			ret = double(HIWORD(v->dwFileVersionMS)) * 100 +
				double(LOWORD(v->dwFileVersionMS)) * 10 +
				double(HIWORD(v->dwFileVersionLS)) +
				double(LOWORD(v->dwFileVersionLS)) * 0.01;
			delete buf;
			return ret;
		}
		else{
			delete buf;
		}
	}

	return 0;
}


BOOL CHidemaruExeExport::init() {

	if (hHideExeHandle) {
		return TRUE;
	}

	GetModuleFileName(NULL, szHidemaruFullPath, _countof(szHidemaruFullPath));
	hm_version = QueryFileVersion(szHidemaruFullPath);

	if (hm_version < 866) {
		MessageBox(NULL, L"�G�ۂ̃o�[�W�������s�����Ă��܂��B\n�G�ۃG�f�B�^ v8.66�ȏオ�K�v�ł��B", L"���", MB_ICONERROR);
		return FALSE;
	}

	// �G�ۖ{�̂Ɋ֐�������̂� 8.66�ȏ�
	hHideExeHandle = LoadLibrary(szHidemaruFullPath);

	if (hHideExeHandle) {
		Hidemaru_GetCurrentWindowHandle = (PFNGetCurrentWindowHandle)GetProcAddress(hHideExeHandle, "Hidemaru_GetCurrentWindowHandle");
		Hidemaru_CheckQueueStatus = (PFNCheckQueueStatus)GetProcAddress(hHideExeHandle, "Hidemaru_CheckQueueStatus");
		Hidemaru_GetDllFuncCalledType = (PFNGetDllFuncCalledType)GetProcAddress(hHideExeHandle, "Hidemaru_GetDllFuncCalledType");
		Hidemaru_GetTotalTextUnicode = (PFNGetTotalTextUnicode)GetProcAddress(hHideExeHandle, "Hidemaru_GetTotalTextUnicode");
		Hidemaru_GetSelectedTextUnicode = (PFNGetSelectedTextUnicode)GetProcAddress(hHideExeHandle, "Hidemaru_GetSelectedTextUnicode");
		Hidemaru_GetLineTextUnicode = (PFNGetLineTextUnicode)GetProcAddress(hHideExeHandle, "Hidemaru_GetLineTextUnicode");
		Hidemaru_GetCursorPosUnicode = (PFNGetCursorPosUnicode)GetProcAddress(hHideExeHandle, "Hidemaru_GetCursorPosUnicode");
		Hidemaru_GetCursorPosUnicodeFromMousePos = (PFNGetCursorPosUnicodeFromMousePos)GetProcAddress(hHideExeHandle, "Hidemaru_GetCursorPosUnicodeFromMousePos");
		Hidemaru_EvalMacro = (PFNEvalMacro)GetProcAddress(hHideExeHandle, "Hidemaru_EvalMacro");
		return TRUE;
	}

	return FALSE;
}

HWND CHidemaruExeExport::GetCurWndHidemaru() {
	if (Hidemaru_GetCurrentWindowHandle) {
		hCurWndHidemaru = Hidemaru_GetCurrentWindowHandle();
	}
	else {
		hCurWndHidemaru = ::GetCurWndHidemaru(hCurWndHidemaru);
	}
	return hCurWndHidemaru;
}

wstring CHidemaruExeExport::GetFileFullPath() {
	HWND hWnd = CHidemaruExeExport::GetCurWndHidemaru();
	wchar_t szBufFileFullPath[MAX_PATH] = L"";

	// ���݂̏G�ۃE�B���h�E�̃t�@�C�����𓾂�B
	LRESULT cwch = SendMessage(hWnd, WM_HIDEMARUINFO, HIDEMARUINFO_GETFILEFULLPATH, (LPARAM)szBufFileFullPath);
	return szBufFileFullPath;
}

wstring CHidemaruExeExport::GetTotalText() {
	HGLOBAL hGlobal = CHidemaruExeExport::Hidemaru_GetTotalTextUnicode();
	if (hGlobal) {
		wchar_t* pwsz = (wchar_t*)GlobalLock(hGlobal);
		wstring text(pwsz); // �R�s�[
		GlobalUnlock(hGlobal);
		GlobalFree(hGlobal); // ���͉̂��
		return text;
	}
	return L"";
}

BOOL CHidemaruExeExport::SetTotalText(wstring text) {
	BOOL success = false;

	auto dll_invocant = CSelfDllInfo::GetInvocantString();

	PushStrVar(text.data());
	wstring cmd =
		L"begingroupundo;\n"
		L"selectall;\n"
		L"insert dllfuncstrw( " + dll_invocant + L"\"PopStrVar\" );\n"
		L"endgroupundo;\n";
	success = CHidemaruExeExport::EvalMacro(cmd);

	return success;
}

BOOL CHidemaruExeExport::AddTotalText(wstring text) {
	BOOL success = false;

	auto dll_invocant = CSelfDllInfo::GetInvocantString();

	PushStrVar(text.data());
	wstring cmd =
		L"begingroupundo;\n"
		L"gofileend;\n"
		L"insert dllfuncstrw( " + dll_invocant + L"\"PopStrVar\" );\n"
		L"endgroupundo;\n";
	success = CHidemaruExeExport::EvalMacro(cmd);

	return success;
}


wstring CHidemaruExeExport::GetSelectedText() {
	HGLOBAL hGlobal = CHidemaruExeExport::Hidemaru_GetSelectedTextUnicode();
	if (hGlobal) {
		wchar_t* pwsz = (wchar_t*)GlobalLock(hGlobal);
		wstring text(pwsz); // �R�s�[
		GlobalUnlock(hGlobal);
		GlobalFree(hGlobal); // ���͉̂��
		return text;
	}
	return L"";
}

wstring CHidemaruExeExport::GetLineText(int lineno) {
	// ���C���i���o�[�̎w�肪�Ȃ���΁A���݂̃J�[�\����lineno
	if (lineno == 0) {
		auto pos = CHidemaruExeExport::GetCursorPos();
		lineno = pos.lineno;
	}
	HGLOBAL hGlobal = CHidemaruExeExport::Hidemaru_GetLineTextUnicode(lineno);
	if (hGlobal) {
		wchar_t* pwsz = (wchar_t*)GlobalLock(hGlobal);
		wstring text(pwsz); // �R�s�[
		GlobalUnlock(hGlobal);
		GlobalFree(hGlobal); // ���͉̂��
		return text;
	}
	return L"";
}


CHidemaruExeExport::HmCursurPos CHidemaruExeExport::GetCursorPos() {
	int nLineNo = -1;
	int nColumn = -1;
	BOOL success = Hidemaru_GetCursorPosUnicode(&nLineNo, &nColumn);
	if (!success) {
		nLineNo = -1;
		nColumn = -1;
	}
	HmCursurPos pos(nLineNo, nColumn);
	return pos;
}

CHidemaruExeExport::HmCursurPos CHidemaruExeExport::GetCursorPosFromMousePos() {
	int nLineNo = -1;
	int nColumn = -1;

	// �Y���̊֐������݂��Ă��鎞�����l���X�V(�G�� 8.73�ȏ�)
	if (Hidemaru_GetCursorPosUnicodeFromMousePos) {
		// ����success��nLineNo�������́AnColumn�̂ǂ��炩�P�����s�����False��Ԃ��Ă��܂��̂ŁA�Ԃ�l�͎g��Ȃ�
		BOOL _ = Hidemaru_GetCursorPosUnicodeFromMousePos(NULL, &nLineNo, &nColumn);
	}
	HmCursurPos pos(nLineNo, nColumn);
	return pos;
}


BOOL CHidemaruExeExport::EvalMacro(wstring cmd) {
	return Hidemaru_EvalMacro(cmd.data());
}



