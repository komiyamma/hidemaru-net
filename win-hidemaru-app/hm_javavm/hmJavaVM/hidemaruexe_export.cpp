#include <windows.h>
#include <tchar.h>
#include <shlwapi.h >

#include "hidemaruexe_export.h"
#include "hidemaruexe_handle.h"
#include "output_debugstream.h"
#include "java_native_methods.h"

#pragma comment(lib, "version.lib")

HWND CHidemaruExeExport::hCurWndHidemaru = NULL;
HMODULE CHidemaruExeExport::hHideExeHandle = NULL;
TCHAR CHidemaruExeExport::szHidemaruFullPath[MAX_PATH] = L"";


CHidemaruExeExport::PFNGetCurrentWindowHandle CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle = NULL;
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
		MessageBox(NULL, L"秀丸のバージョンが不足しています。\n秀丸エディタ v8.66以上が必要です。", L"情報", MB_ICONERROR);
		return FALSE;
	}

	// 秀丸本体に関数があるのは 8.66以上
	hHideExeHandle = LoadLibrary(szHidemaruFullPath);

	if (hHideExeHandle) {
		Hidemaru_GetCurrentWindowHandle = (PFNGetCurrentWindowHandle)GetProcAddress(hHideExeHandle, "Hidemaru_GetCurrentWindowHandle");
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
	} else {
		hCurWndHidemaru = ::GetCurWndHidemaru(hCurWndHidemaru);
	}
	return hCurWndHidemaru;
}

wstring CHidemaruExeExport::GetFileFullPath() {
	HWND hWnd = CHidemaruExeExport::GetCurWndHidemaru();
	wchar_t szBufFileFullPath[MAX_PATH] = L"";

	// 現在の秀丸ウィンドウのファイル名を得る。
	LRESULT cwch = SendMessage(hWnd, WM_HIDEMARUINFO, HIDEMARUINFO_GETFILEFULLPATH, (LPARAM)szBufFileFullPath);
	return szBufFileFullPath;
}

bool CHidemaruExeExport::IsMacroExecuting() {
	HWND hWnd = GetCurWndHidemaru();
	// 875.02から存在するが、それ以前のバージョンも小窓から得られる
	if (hm_version >= 875.02) {
		LRESULT ret = SendMessage(hWnd, WM_ISMACROEXECUTING, NULL, NULL);
		return (bool)ret;
	}
	else {
		HWND hWndHM32Client = FindWindowEx(hWnd, NULL, L"HM32CLIENT", NULL);
		if (hWndHM32Client) {
			LRESULT ret = SendMessage(hWnd, WM_ISMACROEXECUTING, NULL, NULL);
			return (bool)ret;
		}
	}
	return false;
}

wstring CHidemaruExeExport::ExecMacroFromFile(wstring szMacroFileName) {
	// マクロ実行中は別区別
	if (IsMacroExecuting()) {
		return L"-1,HidemaruMacroIsExecutingException,";
	}

	bool isexists = PathFileExists(szMacroFileName.c_str());
	// 対象のファイルは無い
	if (!isexists) {
		return L"0,HidemaruMacroFileNotFoundException,";
	}

	// 875.02から存在する
	if (hm_version < 875.02) {
		return L"0,HidemaruNeedVersionException,";
	}

	HWND hWnd = GetCurWndHidemaru();
	wchar_t wszReturn[4096];
	*(WORD*)wszReturn = _countof(wszReturn);
	LRESULT lRet = SendMessage(hWnd, WM_REMOTE_EXECMACRO_FILE, (WPARAM)wszReturn, (LPARAM)szMacroFileName.c_str());
	// エラーだ
	if (lRet == 0) {
		return L"0,HidemaruMacroEvalException," + wstring(wszReturn);
	}
	else {
		return to_wstring(lRet) + L",," + wstring(wszReturn);
	}
}

wstring CHidemaruExeExport::ExecMacroFromString(wstring cmd) {
	// マクロ実行中は別区別
	if (IsMacroExecuting()) {
		return L"-1,HidemaruMacroIsExecutingException,";
	}

	// 875.02から存在する
	if (hm_version < 875.02) {
		return L"0,HidemaruNeedVersionException,";
	}

	HWND hWnd = GetCurWndHidemaru();
	wchar_t wszReturn[4096];
	*(WORD*)wszReturn = _countof(wszReturn);
	LRESULT lRet = SendMessage(hWnd, WM_REMOTE_EXECMACRO_MEMORY, (WPARAM)wszReturn, (LPARAM)cmd.c_str());
	// エラーだ
	if (lRet == 0) {
		return L"0,HidemaruMacroEvalException," + wstring(wszReturn);
	}
	else {
		return to_wstring(lRet) + L",," + wstring(wszReturn);
	}
}


wstring CHidemaruExeExport::GetTotalText() {
	HGLOBAL hGlobal = CHidemaruExeExport::Hidemaru_GetTotalTextUnicode();
	if (hGlobal) {
		wchar_t* pwsz = (wchar_t*)GlobalLock(hGlobal);
		wstring text(pwsz); // コピー
		GlobalUnlock(hGlobal);
		GlobalFree(hGlobal); // 元のは解放
		return text;
	}
	return L"";
}


wstring CHidemaruExeExport::GetSelectedText() {
	HGLOBAL hGlobal = CHidemaruExeExport::Hidemaru_GetSelectedTextUnicode();
	if (hGlobal) {
		wchar_t* pwsz = (wchar_t*)GlobalLock(hGlobal);
		wstring text(pwsz); // コピー
		GlobalUnlock(hGlobal);
		GlobalFree(hGlobal); // 元のは解放
		return text;
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
		wstring text(pwsz); // コピー
		GlobalUnlock(hGlobal);
		GlobalFree(hGlobal); // 元のは解放
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
