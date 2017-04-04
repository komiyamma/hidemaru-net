#include <windows.h>
#include <tchar.h>

#include "HmExeExport.h"

#pragma comment(lib, "version.lib")


HMODULE CHidemaruExeExport::hHideExeHandle = NULL;
TCHAR CHidemaruExeExport::szHidemaruFullPath[MAX_PATH] = L"";

CHidemaruExeExport::PFNGetTotalTextUnicode CHidemaruExeExport::Hidemaru_GetTotalTextUnicode = NULL;

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


CHidemaruExeExport::CHidemaruExeExport() {

	if (hHideExeHandle) {
		return;
	}

	GetModuleFileName(NULL, szHidemaruFullPath, _countof(szHidemaruFullPath));
	hm_version = QueryFileVersion(szHidemaruFullPath);

	if (hm_version < 866) {
		MessageBox(NULL, L"秀丸のバージョンが不足しています。\n秀丸エディタ v8.66以上が必要です。", L"情報", MB_ICONERROR);
		return;
	}

	// 秀丸本体に関数があるのは 8.66以上
	hHideExeHandle = LoadLibrary(szHidemaruFullPath);

	if (hHideExeHandle) {
		Hidemaru_GetTotalTextUnicode = (PFNGetTotalTextUnicode)GetProcAddress(hHideExeHandle, "Hidemaru_GetTotalTextUnicode");
		return;
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



