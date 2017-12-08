/*
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the MIT License
 */

#include <windows.h>
#include <tchar.h>

#include "hidemaruexe_export.h"
#include "hidemaruexe_handle.h"

#pragma comment(lib, "version.lib")

PFNGetDllFuncCalledType Hidemaru_GetDllFuncCalledType = NULL;


HWND CHidemaruExeExport::hCurWndHidemaru = NULL;
HMODULE CHidemaruExeExport::hHideExeHandle = NULL;
TCHAR CHidemaruExeExport::szHidemaruFullPath[MAX_PATH] = L"";

CHidemaruExeExport::PFNGetCurrentWindowHandle CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle = NULL;
CHidemaruExeExport::PFNGetTotalTextUnicode CHidemaruExeExport::Hidemaru_GetTotalTextUnicode = NULL;

double CHidemaruExeExport::hm_version = 0;
double CHidemaruExeExport::QueryFileVersion(TCHAR* path) {
	VS_FIXEDFILEINFO* v;
	DWORD dwZero = 0;
	UINT len;
	DWORD sz = GetFileVersionInfoSize(path, &dwZero);
	if (sz) {
		void* buf = new char[sz];
		GetFileVersionInfo(path, dwZero, sz, buf);

		if (VerQueryValue(buf, L"\\", (LPVOID*)&v, &len)) {
			double ret = 0;
			ret = double(HIWORD(v->dwFileVersionMS)) * 100 +
				double(LOWORD(v->dwFileVersionMS)) * 10 +
				double(HIWORD(v->dwFileVersionLS)) +
				double(LOWORD(v->dwFileVersionLS)) * 0.01;
			delete buf;
			return ret;
		}
		else {
			delete buf;
		}
	}

	return 0;
}


CHidemaruExeExport::CHidemaruExeExport() {

	if (hHideExeHandle) {
		return;
	}

	GetModuleFileName(NULL, szHidemaruFullPath, _countof(szHidemaruFullPath));
	hm_version = QueryFileVersion(szHidemaruFullPath);

	if (hm_version < 866) {
		MessageBox(NULL, L"�G�ۂ̃o�[�W�������s�����Ă��܂��B\n�G�ۃG�f�B�^ v8.66�ȏオ�K�v�ł��B", L"���", MB_ICONERROR);
		return;
	}

	// �G�ۖ{�̂Ɋ֐�������̂� 8.66�ȏ�
	hHideExeHandle = LoadLibrary(szHidemaruFullPath);

	if (hHideExeHandle) {
		Hidemaru_GetDllFuncCalledType = (PFNGetDllFuncCalledType)GetProcAddress(hHideExeHandle, "Hidemaru_GetDllFuncCalledType");
		Hidemaru_GetCurrentWindowHandle = (PFNGetCurrentWindowHandle)GetProcAddress(hHideExeHandle, "Hidemaru_GetCurrentWindowHandle");
		Hidemaru_GetTotalTextUnicode = (PFNGetTotalTextUnicode)GetProcAddress(hHideExeHandle, "Hidemaru_GetTotalTextUnicode");
		return;
	}
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


CHidemaruExeExport init;