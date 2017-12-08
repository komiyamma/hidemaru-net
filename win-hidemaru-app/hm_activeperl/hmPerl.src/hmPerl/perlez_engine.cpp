/*
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */



#include <shlwapi.h>
#include <vector>

#include "perlez_engine.h"
#include "self_dll_info.h"
#include "convert_string.h"
#include "output_debugstream.h"


using namespace std;

#pragma comment(lib, "shlwapi.lib")

extern vector<wstring> wstring_split(const std::wstring& s, const std::wstring& delim); // 文字列を指定文字で分割


// staticの初期化
HMODULE CPerlEzEngine::hPerlEzDll = NULL;
PERLEZHANDLE CPerlEzEngine::engine = NULL;


CPerlEzEngine::CPerlEzEngine() {

	//-------------------------------------------------------------------------
	// ActivePerlなんだからPathが通っているよね？ という前提の仕組みです >> PerlEz
	//-------------------------------------------------------------------------
	const TCHAR szLibName[MAX_PATH] = _T("PerlEz.DLL");

	// XMLによる指定があるか？
	wstring wstrPerlEzPath = GetPerlEzPath();
	if (wstrPerlEzPath.length() > 0) {
		hPerlEzDll = LoadLibrary( wstrPerlEzPath.c_str() );
		if (!hPerlEzDll) {
			MessageBox(NULL, L"指定のPerlEZ.dllを読み込むことが出来ません", L"エラー", NULL);
		}
	}
	// 指定は無い
	else {
		hPerlEzDll = LoadLibrary(szLibName);

	}

	if (!hPerlEzDll) {
		// ちょっと探してみる

		// 環境変数のパスを取得
		TCHAR szEnvPATH[4096] = { 0 };
		GetEnvironmentVariable(_T("PATH"), szEnvPATH, _countof(szEnvPATH));

		// 「;」で割る
		vector<wstring> path_list = wstring_split(szEnvPATH, _T(";"));

		// 先頭から…
		for (wstring path : path_list) {
			auto full_path = path + _T("\\") + szLibName;
			// PerlEz.dllを見つけたら
			if ( PathFileExists( full_path.c_str() ) ) {
				// フルパス状態でロード
				hPerlEzDll = LoadLibrary(full_path.c_str());
				if (hPerlEzDll) {
					break;
				}
			}
		}
	}
	if (hPerlEzDll) {

		//-------------------------------------------------------------------------
		// 以下は10年ぐらい変更が無いので、チェックするだけ無駄なので、関数は存在してると信じる。
		//-------------------------------------------------------------------------
		this->PerlEzCreate = (TPerlEzCreate)GetProcAddress(hPerlEzDll, "PerlEzCreate");
		this->PerlEzDelete = (TPerlEzDelete)GetProcAddress(hPerlEzDll, "PerlEzDelete");

		this->PerlEzCall = (TPerlEzCall)GetProcAddress(hPerlEzDll, "PerlEzCall");
		this->PerlEzEvalString = (TPerlEzEvalString)GetProcAddress(hPerlEzDll, "PerlEzEvalString");

		this->PerlEzSetMagicScalarName = (TPerlEzSetMagicScalarName)GetProcAddress(hPerlEzDll, "PerlEzSetMagicScalarName");
		this->PerlEzSetMagicScalarFunctions = (TPerlEzSetMagicScalarFunctions)GetProcAddress(hPerlEzDll, "PerlEzSetMagicScalarFunctions");


		//-------------------------------------------------------------------------
		// dllの名前に対応する形で、.pmの名前を決める。
		// デフォルトだと、hmPerl.dllとhmPerl.pmとなる

		//-------------------------------------------------------------------------
		// 自分自身のdllの名前をまずはコピーして…
		_tcscpy_s(szPerlCreateFileFullPath, CSelfDllInfo::szSelfModuleFullPath);
		//-------------------------------------------------------------------------
		// 拡張子を.pmへと変更したものを、読みこむ。
		PathRenameExtension(szPerlCreateFileFullPath, L".pm");

		//-------------------------------------------------------------------------
		// PerlEzが古くからあるので、asciiというかマルチバイトのファイルしか読み込めない
		if (PathFileExists(szPerlCreateFileFullPath)) {
			string cp932_full_path = utf16_to_cp932(szPerlCreateFileFullPath);
			this->engine = this->PerlEzCreate(cp932_full_path.data(), NULL);
			if (!this->engine) {
				wstring errmsg = wstring(szPerlCreateFileFullPath) + L"\n関連でトラブルが発生しました。";
				MessageBox(NULL, errmsg.data(), errmsg.data(), MB_ICONERROR);
				OutputDebugStream(errmsg);
			}
		}
	}

	else {
		MessageBox(NULL, L"ActivePerl x86版を認識できません", L"ActivePerl x86版を認識できません", MB_ICONERROR);
	}
}




CPerlEzEngine::~CPerlEzEngine() {

	//-------------------------------------------------------------------------
	// エンジンを破棄してから…
	//-------------------------------------------------------------------------
	if (engine) {
		PerlEzDelete(engine);
		engine = NULL;
	}

	//-------------------------------------------------------------------------
	// dllを破棄。
	//-------------------------------------------------------------------------
	if (hPerlEzDll) {
		FreeLibrary(hPerlEzDll);
		hPerlEzDll = NULL;
	}
}


BOOL CPerlEzEngine::IsValid() {
	return (hPerlEzDll != NULL);
}


BOOL CPerlEzEngine::IsReturnBufferSeemsEvalError(char *utf8_buffer) {
	// 何かエラーっぽい文字列が入っているぞ…
	if (strstr(utf8_buffer, "(eval") && strstr(utf8_buffer, "at (") && strstr(utf8_buffer, ") line")) {
		return true;
	}
	return false;
}


// 文字列を指定文字で分割
static vector<wstring> wstring_split(const std::wstring& s, const std::wstring& delim)
{
	vector<wstring> result;
	result.clear();

	using wstring = std::wstring;
	wstring::size_type pos = 0;

	while (pos != wstring::npos)
	{
		wstring::size_type p = s.find(delim, pos);

		if (p == wstring::npos)
		{
			result.push_back(s.substr(pos));
			break;
		}
		else {
			result.push_back(s.substr(pos, p - pos));
		}

		pos = p + delim.size();
	}

	return result;
}


// エラーの位置の情報を出す
CPerlEzEngine::TEvalErrorInfo CPerlEzEngine::GetEvalErrorInfo(wstring strAllSrcCode, char *utf8_buffer) {
	TEvalErrorInfo errInfo;

	// 何かエラーっぽい文字列が入っているぞ…
	if (this->IsReturnBufferSeemsEvalError(utf8_buffer)) {

		//-------------------------------------------------------------------------
		// 正規表現で、正確に行と位置を抽出する
		//-------------------------------------------------------------------------
		//  at (eval 9) line 2
		std::wregex wre( L"at \\(eval (\\d+)\\) line (\\d+)" );

		std::wsmatch wmt; // match_results

		auto result = std::regex_search(utf8_to_utf16(utf8_buffer), wmt, wre);
		wstring w1 = wmt[1];
		wstring w2 = wmt[2];

		//-------------------------------------------------------------------------
		// 数値に変換できるかどうかトライ
		//-------------------------------------------------------------------------
		int column = -1;
		int lineno = -1;
		try {
			column = std::stoi(w1);
			lineno = std::stoi(w2);
		}
		catch (...) {
			column = -1;
			lineno = -1;
		}

		//-------------------------------------------------------------------------
		// Perlコード全体を改行で分割して…
		//-------------------------------------------------------------------------
		auto result_list = wstring_split(strAllSrcCode, L"\n");

		//-------------------------------------------------------------------------
		// エラーがあったとされる行の文字列を入れておく
		//-------------------------------------------------------------------------
		int n = lineno - 1;
		if (0 <= n && n < (int)result_list.size()) {
			errInfo.errorlinecode = result_list[n];
		}

		errInfo.column = column;
		errInfo.lineno = lineno;
		errInfo.HasError = true;
		
	}
	else {
		errInfo.HasError = false;
	}
	return errInfo;
}
