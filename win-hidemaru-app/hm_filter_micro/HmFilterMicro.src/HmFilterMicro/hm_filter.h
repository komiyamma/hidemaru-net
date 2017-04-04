#pragma once

#include <windows.h>
#include <string>

#include "convert_string.h"
#include "output_debugstream.h"


using namespace std;

#define MACRO_DLL extern "C" __declspec(dllexport) 

//---------------------------------------------------------------------------------------------------
// hwnHidemaru   : 秀丸のウィンドウハンドル
// pwszIn        : wchar_t* 型で 変換対象となっている文字列が全部渡ってくる。
// pszParamBuffer: 「秀丸マクロのfilter関数」で渡すことになる第３引数の文字列。
//                これは自分で自由に自分で決める、パラメータみたいなもの。
//                通常は「0」と「1」の並びとすることが多いが、自分自身でParseするので何でも良いだろう。
// 
//                例えば、秀丸の「Sort変換モジュール」の場合は、
//                fReverse     = pszParamBuffer[0] == '1' or '0';
//                fIgnoreSpace = pszParamBuffer[1] == '1' or '0';
//                fIgnoreCase  = pszParamBuffer[2] == '1' or '0';
//                fRealNumber  = pszParamBuffer[3] == '1' or '0';
//
//                又、パラメータを読み取るだけではなく、書き込んでおくことも可能。
//                書き込んでおいたものは、「キー操作の記録」をすると、秀丸エディタがこのパラメータを覚える。
//                キー操作の再生をすると、秀丸エディタで覚えていたパラメータを付けて変換モジュールを呼び出すようになり、
//                ダイアログを出さずに前回と同じ選択で実行が出来るようにくみ上げることが可能になっています。
//
// cbParamBuffer : pszParamの秀丸本体側のバッファーサイズ。260。
//---------------------------------------------------------------------------------------------------
using HIDEMARUFILTERFUNC = HGLOBAL(_cdecl *)( HWND hwndHidemaru, const wchar_t* pwszIn, char* pszParamBuffer, int cbParamBuffer);


struct HIDEMARUFILTERINFO {
	UINT                cbStructSize;
	const char*         pszExportName;
	const char*         pszNameJapan;
	const char*         pszNameUs;
	BYTE                chAccel;        // アクセラレータキー
	BYTE                fMustLineUnit;  // 行単位でないと実行できないフラグ
	BYTE                abReserve[2];
};


#define WM_HIDEMARUINFO (WM_USER + 181)

#define HIDEMARUINFO_GETTABWIDTH	0
#define HIDEMARUINFO_GETSTARTCOLUMN 1
#define HIDEMARUINFO_GETSPACETAB	2



// utf16→globalへ。これは解放を自分でやっていないので秀丸変換モジュール専用
HGLOBAL utf16_to_hm_filter_global(const wstring& wstr_text);

// 普通の書き方のバージョンを秀丸が要求する形のバージョン数値へ
int double_ver_to_hm_filter_ver(const double ver);
