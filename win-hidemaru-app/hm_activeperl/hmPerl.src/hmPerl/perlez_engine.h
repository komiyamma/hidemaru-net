#pragma once
#include <tchar.h>

#include <windows.h>
#include <regex>
#include <string>

#include "perlez.h"

using namespace std;


class CPerlEzEngine {

private:
	//========================================================================
	// PerlEz.dllのdll自体のハンドル
	//========================================================================
	static HMODULE hPerlEzDll;
	// セッティング用のXMLを用意してある場合に、PerlEzのフルパスを指定することが出来る。
	static wstring GetPerlEzPath();
public:
	//-------------------------------------------------------------------------
	// PerlEzが管理するPerlスクリプトエンジンのハンドル
	static PERLEZHANDLE engine;

private:
	//========================================================================
	/// PerlEzからExportされているいくつかの関数群の型保持
	//========================================================================

	//-------------------------------------------------------------------------
	// エンジンの生成
	using TPerlEzCreate = decltype(::PerlEzCreate)*;

	//-------------------------------------------------------------------------
	// エンジンの破棄
	using TPerlEzDelete = decltype(::PerlEzDelete)*;

	//-------------------------------------------------------------------------
	// Perlグローバルスコープ上にある関数を引数付で呼び出す
	using TPerlEzCall = decltype(::PerlEzCall)*;

	//-------------------------------------------------------------------------
	// Perlグローバルスコープ上に対して、Perlスクリプトをその場で実行し、評価結果を得る
	using TPerlEzEvalString = decltype(::PerlEzEvalString)*;

	//-------------------------------------------------------------------------
	// グローバル変数に対して、「取得」と「代入」の度に、C言語のフック関数が呼び出されるような
	// 特別なグローバル変数を作りだす。
	using TPerlEzSetMagicScalarName = decltype(::PerlEzSetMagicScalarName)*;

	//-------------------------------------------------------------------------
	// 上記すべての特別なグローバル変数で、共用するフック用関数を設定する。
	// 「読み取り時用」と「書き込み時用」の２種類
	// 第２引数にグローバル変数の変数名自体が渡ってくるので、それで処理を振り分ける。
	using TPerlEzSetMagicScalarFunctions = decltype(::PerlEzSetMagicScalarFunctions)*;

public:
	//========================================================================
	/// PerlEzからExportされているいくつかの関数群
	//========================================================================
	TPerlEzCreate     PerlEzCreate; // 第１引数を""にすれば、ファイルを読まなくても良い。
	TPerlEzDelete     PerlEzDelete;

	TPerlEzCall       PerlEzCall;
	TPerlEzEvalString PerlEzEvalString;

	TPerlEzSetMagicScalarName      PerlEzSetMagicScalarName;
	TPerlEzSetMagicScalarFunctions PerlEzSetMagicScalarFunctions;

public:
	CPerlEzEngine();
	~CPerlEzEngine();

private:
	//========================================================================
	// IronPython/DLRののように、完成度が高い「誰もがベストと思うであろう表記」が存在するのとは異なり
	// Perlの場合は、好みで記述が著しくわかれてしまうため、hm->Macro->***やhm->Edit->***といった
	// 表記をhmPerl.pmという形で外へと出して、自由にカスタム出来るようにした。
	//========================================================================

	//========================================================================
	// hmPerl.pmのフルパス
	TCHAR szPerlCreateFileFullPath[MAX_PATH];


public:
	//-------------------------------------------------------------------------
	// エンジンは有効(読み込み済)かどうか。
	static BOOL IsValid();

	// 対象の文字列はパールEvalエラーぽい。
	BOOL IsReturnBufferSeemsEvalError(char *utf8_buffer);

	struct TEvalErrorInfo {
		bool HasError;		// エラーがあったかどうか。
		int lineno;			// エラー行
		int column;			// エラー位置
		wstring errorlinecode;	// エラーが出た行の文字列
	};

	// エラーの位置と情報を分析
	TEvalErrorInfo GetEvalErrorInfo(wstring strAllSrcCode, char *utf8_buffer);
};


// 関数の追加説明

/*
char *pfilename = "C:/test.pl"

-------------------------------------------------------------------------------------
int ret = PerlEzCreateOpt(pfilename, "", "market C:\\usr\\perl\\bin\\corpus.txt")
-------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------
int ret = PerlEzCreate("", ""); のようにすれば、ファイルは読まなくても良い。
-------------------------------------------------------------------------------------

*/