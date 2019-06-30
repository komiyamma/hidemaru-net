/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the MIT License
 */

#include "hm_filter.h"

#include <algorithm>

using namespace std;



//---------------------------------------------------------------------------------------------------
// 自分自身の定義全体とでも呼べるものを返す。上のaFilterInfo。
// これはこのままコピペで良い
//---------------------------------------------------------------------------------------------------
MACRO_DLL
HIDEMARUFILTERINFO* EnumHidemaruFilter() {
	//---------------------------------------------------------------------------------------------------
	// 秀丸指定のフォーマット。
	// ① 自身の構造体サイズ
	// ② 呼んでCallBackしてもらう関数名
	// ③ メニューに出る日本語名
	// ④ メニューに出る英語名
	// ⑤ メニューのショートカットキー
	//---------------------------------------------------------------------------------------------------
	static HIDEMARUFILTERINFO aFilterInfo[14] = {
		{ sizeof(HIDEMARUFILTERINFO), "MyTestFunc", "日本語でのこの変換モジュール名", "English Convert ModuleName", 'T', 0, 0, 0 },
		{ 0, NULL, NULL, NULL, NULL, 0, 0, 0 }
	};
	return aFilterInfo;
}


//---------------------------------------------------------------------------------------------------
// この変換モジュールのバージョン
//---------------------------------------------------------------------------------------------------
MACRO_DLL
DWORD HidemaruFilterGetVersion() {
	double version = 1.02;
	return double_ver_to_hm_filter_ver(version);
}


//---------------------------------------------------------------------------------------------------
// hwnHidemaru   : 秀丸のウィンドウハンドル
//
// pwszIn        : wchar_t* 型で 変換対象となっている文字列が全部渡ってくる。
//                 通常は秀丸エディタで現在編集中の「選択しているテキスト」が渡ってくることになる。
//                 秀丸マクロであれば、「filter関数」を使って第４引数で直接渡すことも可能。
//
// pszParamBuffer: これは自分で自由に自分で決める、パラメータみたいなもの。
//                  複数の使い道がある。
//                 ①まず、秀丸エディタの変換のメニューから選んだ場合は、NULLしか渡ってこない。
//                 ②「秀丸マクロのfilter文やfilter関数」で第３引数に文字列を渡した場合は、そのまま渡ってくる。
//                  この場合は、該当の秀丸マクロのファイル(.mac)の文字コードが不明なため、「英数字」だけが望ましい。
//
//                  通常は「0」と「1」の並びとすることが多い。自分自身で「何番めの文字はどういう意味に使う」と決めることになる。
// 
//                  例えば、秀丸の「Sort変換モジュール」の場合は、
//                  fReverse     = pszParamBuffer[0] == '1' or '0';
//                  fIgnoreSpace = pszParamBuffer[1] == '1' or '0';
//                  fIgnoreCase  = pszParamBuffer[2] == '1' or '0';
//                  fRealNumber  = pszParamBuffer[3] == '1' or '0';
//
//                  を並べて、"1101" などといった形となっており、filter関数などで、そのように渡すことが可能である。
// 
//                 ③パラメータを読み取るだけではなく、書き込んでおくことも可能。
//                　書き込んでおいたものは、「秀丸マクロのキー操作の記録」をすると、秀丸エディタがこのパラメータを覚えおり、
//                  「キー操作の再生」をすると、そのパラメータをpszParamBufferに渡した状態で、この関数を呼び出すことになる。
// 
//                  このような仕組みがあることで、「マクロのキー操作時の記録」ではダイアログを出して値を選択し、「キー操作再生時」にはダイアログで選んだ値をそのまま使うといったことが可能となっている。
//
// cbParamBuffer : pszParamの秀丸本体側のバッファーサイズ。260。
//---------------------------------------------------------------------------------------------------
MACRO_DLL
HGLOBAL MyTestFunc( HWND hwndHidemaru, const wchar_t *pwszIn, char *pszParamBuffer, int cbParamBuffer ) {

	// まず、元のデータをwstring型で確保。
	wstring wstr_text = pwszIn;

	//---------------------------------------------------------------------------------------------------
	// ここで、いろいろ文字列を加工する
	//---------------------------------------------------------------------------------------------------

	// パラメータの最初の要素
	char mode = pszParamBuffer[0];

	switch (mode) {
	case NULL: {

		// 「pszParamBuffer」へと書き込んでおくと、
		// (※)「マクロ記録」による「キー再生」の際、pszParamBuffer[0]に'2'が入った状態で、秀丸がこの関数を呼んでくれる。
		// よって、「マクロのキー再生」時には「case '2'」となるので、「大文字」へと変換されることとなる。
		pszParamBuffer[0] = '2';

		// 小文字に変えてみる
		std::transform(wstr_text.begin(), wstr_text.end(), wstr_text.begin(), ::tolower);
		break;
	}

	case '1':{
		// 小文字に変えてみる
		std::transform(wstr_text.begin(), wstr_text.end(), wstr_text.begin(), ::tolower);
		break;
	}

	case '2':
		// 大文字に変えてみる
		std::transform(wstr_text.begin(), wstr_text.end(), wstr_text.begin(), ::toupper);
		break;
	}

	// デバッグモニターに出力(cp932を超える分は潰れる)
	OutputDebugStream(wstr_text);

	return utf16_to_hm_filter_global(wstr_text);
}




