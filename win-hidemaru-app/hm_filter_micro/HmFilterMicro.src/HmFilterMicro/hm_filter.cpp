#define _CRT_SECURE_NO_WARNINGS
#include "hm_filter.h"

using namespace std;

HGLOBAL utf16_to_hm_filter_global(const wstring& wstr_text) {
	// wstring型なので、必ず２倍。
	size_t cb = (wstr_text.length() + 1) * 2;

	HGLOBAL	hmemDst = GlobalAlloc(GMEM_MOVEABLE, cb);
	if (hmemDst == nullptr) {
		MessageBox(NULL, L"メモリが十分確保できない", L"メモリが十分確保できない", NULL);
		return nullptr;
	}

	// ロック
	wchar_t* pwchDest = (wchar_t*)GlobalLock(hmemDst);
	// コピペ
	wcscpy(pwchDest, wstr_text.c_str());
	// アンロック
	GlobalUnlock(pwchDest);
	return pwchDest;
}


//---------------------------------------------------------------------------------------------------
// 普通のバージョン数値 → 秀丸変換モジュールタイプのバージョンに変換する
//---------------------------------------------------------------------------------------------------
int double_ver_to_hm_filter_ver(const double ver) {
	// マイナーバージョン部分を出して…
	double minor = ver - (int)ver;

	// メジャーバージョン部分を出して
	int major = int(ver - minor);

	// 秀丸が欲する形へと変換
	return (major << 16) + int(100 * minor);
}
