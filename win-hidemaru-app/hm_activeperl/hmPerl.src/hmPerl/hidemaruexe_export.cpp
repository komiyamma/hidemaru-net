/*
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#include <windows.h>
#include <tchar.h>

#include "hidemaruexe_export.h"

#pragma comment(lib, "version.lib")

HMODULE CHidemaruExeExport::hHideExeHandle = NULL;
TCHAR CHidemaruExeExport::szHidemaruFullPath[MAX_PATH] = L"";

CHidemaruExeExport::PFNGetDllFuncCalledType CHidemaruExeExport::Hidemaru_GetDllFuncCalledType = NULL;
CHidemaruExeExport::PFNGetTotalTextUnicode CHidemaruExeExport::Hidemaru_GetTotalTextUnicode = NULL;
CHidemaruExeExport::PFNGetSelectedTextUnicode CHidemaruExeExport::Hidemaru_GetSelectedTextUnicode = NULL;
CHidemaruExeExport::PFNGetLineTextUnicode CHidemaruExeExport::Hidemaru_GetLineTextUnicode = NULL;
CHidemaruExeExport::PFNAnalyzeEncoding CHidemaruExeExport::Hidemaru_AnalyzeEncoding = NULL;
CHidemaruExeExport::PFNLoadFileUnicode CHidemaruExeExport::Hidemaru_LoadFileUnicode = NULL;
CHidemaruExeExport::PFNGetCursorPosUnicode CHidemaruExeExport::Hidemaru_GetCursorPosUnicode = NULL;
CHidemaruExeExport::PFNGetCursorPosUnicodeFromMousePos CHidemaruExeExport::Hidemaru_GetCursorPosUnicodeFromMousePos = NULL;
CHidemaruExeExport::PFNEvalMacro CHidemaruExeExport::Hidemaru_EvalMacro = NULL;
CHidemaruExeExport::PFNGetCurrentWindowHandle CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle = NULL;
// アウトプットパネル
CHidemaruExeExport::PFNHmOutputPane_Output CHidemaruExeExport::HmOutputPane_Output = NULL;
CHidemaruExeExport::PFNHmOutputPane_Push CHidemaruExeExport::HmOutputPane_Push = NULL;
CHidemaruExeExport::PFNHmOutputPane_Pop CHidemaruExeExport::HmOutputPane_Pop = NULL;
CHidemaruExeExport::PFNHmOutputPane_GetWindowHandle CHidemaruExeExport::HmOutputPane_GetWindowHandle = NULL;
CHidemaruExeExport::PFNHmOutputPane_SetBaseDir CHidemaruExeExport::HmOutputPane_SetBaseDir = NULL;

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
			delete[] buf;
			return ret;
		}
		else{
			delete[] buf;
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
		MessageBox(NULL, L"秀丸のバージョンが不足しています。\n秀丸エディタ v8.66以上が必要です。", L"情報", MB_ICONERROR);
		return FALSE;
	}

	// 秀丸本体に関数があるのは 8.66以上
	hHideExeHandle = LoadLibrary(szHidemaruFullPath);

	if (hHideExeHandle) {
		Hidemaru_GetDllFuncCalledType = (PFNGetDllFuncCalledType)GetProcAddress(hHideExeHandle, "Hidemaru_GetDllFuncCalledType");
		Hidemaru_GetTotalTextUnicode = (PFNGetTotalTextUnicode)GetProcAddress(hHideExeHandle, "Hidemaru_GetTotalTextUnicode");
		Hidemaru_GetSelectedTextUnicode = (PFNGetSelectedTextUnicode)GetProcAddress(hHideExeHandle, "Hidemaru_GetSelectedTextUnicode");
		Hidemaru_GetLineTextUnicode = (PFNGetLineTextUnicode)GetProcAddress(hHideExeHandle, "Hidemaru_GetLineTextUnicode");
		Hidemaru_AnalyzeEncoding = (PFNAnalyzeEncoding)GetProcAddress(hHideExeHandle, "Hidemaru_AnalyzeEncoding");
		Hidemaru_LoadFileUnicode = (PFNLoadFileUnicode)GetProcAddress(hHideExeHandle, "Hidemaru_LoadFileUnicode");
		Hidemaru_GetCursorPosUnicode = (PFNGetCursorPosUnicode)GetProcAddress(hHideExeHandle, "Hidemaru_GetCursorPosUnicode");
		Hidemaru_GetCursorPosUnicodeFromMousePos = (PFNGetCursorPosUnicodeFromMousePos)GetProcAddress(hHideExeHandle, "Hidemaru_GetCursorPosUnicodeFromMousePos");
		Hidemaru_EvalMacro = (PFNEvalMacro)GetProcAddress(hHideExeHandle, "Hidemaru_EvalMacro");

		Hidemaru_GetCurrentWindowHandle = (PFNGetCurrentWindowHandle)GetProcAddress(hHideExeHandle, "Hidemaru_GetCurrentWindowHandle");

		// 少なくともGetWindowsCurrentHandleが無いと、役に立たない
		if (Hidemaru_GetCurrentWindowHandle) {
			// hidemaru.exeのディレクトリを求める
			wchar_t hidemarudir[512] = L"";
			wcscpy_s(hidemarudir, szHidemaruFullPath);
			PathRemoveFileSpec(hidemarudir);

			// ディレクトリある？ （まぁあるよね）
			if (PathFileExists(hidemarudir)) {
				// HmOutputPane.dllがあるかどうか。
				wstring hmoutputpane_fullpath = wstring(hidemarudir) + wstring(L"\\HmOutputPane.dll");
				HMODULE hHmOutputPaneDLL = LoadLibrary(hmoutputpane_fullpath.data());
				// あれば、Output関数をセッティングしておく
				if (hHmOutputPaneDLL) {
					HmOutputPane_Output = (PFNHmOutputPane_Output)GetProcAddress(hHmOutputPaneDLL, "Output");
					HmOutputPane_Push = (PFNHmOutputPane_Push)GetProcAddress(hHmOutputPaneDLL, "Push");
					HmOutputPane_Pop = (PFNHmOutputPane_Pop)GetProcAddress(hHmOutputPaneDLL, "Pop");
					HmOutputPane_GetWindowHandle = (PFNHmOutputPane_GetWindowHandle)GetProcAddress(hHmOutputPaneDLL, "GetWindowHandle");
					HmOutputPane_SetBaseDir = (PFNHmOutputPane_SetBaseDir)GetProcAddress(hHmOutputPaneDLL, "SetBaseDir");
				}
			}
		}

		return TRUE;
	}

	return FALSE;
}


wstring CHidemaruExeExport::GetTotalText() {
	HGLOBAL hGlobal = CHidemaruExeExport::Hidemaru_GetTotalTextUnicode();
	if (hGlobal) {
		wchar_t* pwsz = (wchar_t*)GlobalLock(hGlobal);
		if (pswz) {
			wstring text(pwsz); // コピー
			GlobalUnlock(hGlobal);
			GlobalFree(hGlobal); // 元のは解放
			return text;
		}
	}
	return L"";
}


wstring CHidemaruExeExport::GetSelectedText() {
	HGLOBAL hGlobal = CHidemaruExeExport::Hidemaru_GetSelectedTextUnicode();
	if (hGlobal) {
		wchar_t* pwsz = (wchar_t*)GlobalLock(hGlobal);
		if (pswz) {
			wstring text(pwsz); // コピー
			GlobalUnlock(hGlobal);
			GlobalFree(hGlobal); // 元のは解放
			return text;
		}
	}
	return L"";
}

wstring CHidemaruExeExport::GetLineText(int lineno) {
	// ラインナンバーの指定がなければ、現在のカーソルのlineno
	if (lineno == 0) {
		auto pos = CHidemaruExeExport::GetCursorPos();
		lineno = pos.lineno;
	}
	HGLOBAL hGlobal = CHidemaruExeExport::Hidemaru_GetLineTextUnicode(lineno);
	if (hGlobal) {
		wchar_t* pwsz = (wchar_t*)GlobalLock(hGlobal);
		if (pwsz) {
			wstring text(pwsz); // コピー
			GlobalUnlock(hGlobal);
			GlobalFree(hGlobal); // 元のは解放
			return text;
		}
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

CHidemaruExeExport::HmMousePos CHidemaruExeExport::GetCursorPosFromMousePos() {
	POINT point;
	int s = ::GetCursorPos(&point);
	if (!s) {
		point.x = -1;
		point.y = -1;
	}
	int nLineNo = -1;
	int nColumn = -1;

	// 該当の関数が存在している時だけ値を更新(秀丸 8.73以上)
	if (Hidemaru_GetCursorPosUnicodeFromMousePos) {
		// このsuccessはnLineNoもしくは、nColumnのどちらか１つが失敗するとFalseを返してしまうので、返り値は使わない
		BOOL _ = Hidemaru_GetCursorPosUnicodeFromMousePos(NULL, &nLineNo, &nColumn);
	}
	HmMousePos pos(point.x, point.y, nLineNo, nColumn);
	return pos;
}


BOOL CHidemaruExeExport::EvalMacro(wstring cmd) {
	return Hidemaru_EvalMacro(cmd.data());
}

int CHidemaruExeExport::AnalyzeEncoding(wstring filename) {
	// 該当の関数が存在している時だけ値を更新(秀丸 8.90以上)
	if (Hidemaru_AnalyzeEncoding) {
		return Hidemaru_AnalyzeEncoding(filename.data(), NULL, NULL);
	}
	else {
		return 0;
	}
}

wstring CHidemaruExeExport::LoadFileUnicode(wstring filename, int nHmEncode, UINT* pcwchOut, DWORD_PTR lParam1, DWORD_PTR lParam2, bool* success) {

	if (Hidemaru_LoadFileUnicode) {
		HGLOBAL hGlobal = CHidemaruExeExport::Hidemaru_LoadFileUnicode(filename.data(), nHmEncode, pcwchOut, lParam1, lParam2);
		if (hGlobal) {
			wchar_t* pwsz = (wchar_t*)GlobalLock(hGlobal);
			if (pswz) {
				wstring text(pwsz); // コピー
				GlobalUnlock(hGlobal);
				GlobalFree(hGlobal); // 元のは解放
				*success = true;
				return text;
			}
		}
	}

	*success = false;

	// ここからは下だということは、読み込みに失敗している

	if (pcwchOut) {
		*pcwchOut = 0;
	}

	return L"";
}

HWND CHidemaruExeExport::OutputPane_GetWindowHanndle() {

	// ちゃんと関数がある時だけ
	if (CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle) {
		HWND hHidemaruWindow = CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle();
		if (CHidemaruExeExport::HmOutputPane_GetWindowHandle) {
			return CHidemaruExeExport::HmOutputPane_GetWindowHandle(hHidemaruWindow);
		}
	}

	return NULL;
}


