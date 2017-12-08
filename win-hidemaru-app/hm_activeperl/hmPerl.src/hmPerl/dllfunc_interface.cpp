﻿/*
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#include <stdio.h>
#include <windows.h>

#include "convert_string.h"
#include "perlez_engine.h"
#include "perlez_magical_scalar.h"
#include "hidemaruexe_export.h"
#include "self_dll_info.h"

#include "dllfunc_interface.h"
#include "dllfunc_interface_internal.h"
#include "output_debugstream.h"

using namespace std;



CPerlEzEngine *module = NULL;
static int CreateScope() {

	// すでに生成済みである。
	if (CPerlEzEngine::IsValid()) {
		return TRUE;
	}

	module = new CPerlEzEngine();
	if (!module->IsValid()) {
		return FALSE;
	}

	CPerlEzMagicalScalar::BindMagicalScalarFunctions(module);
	return TRUE;
}



//------------------------------------------------------------------------------------
MACRO_DLL intHM_t GetNumVar(const TCHAR *sz_full_var_name) {
	if (CreateScope() == 0)
	{
		return 0;
	}

	// DoStringされる度にdllのBindの在り方を確認更新する。
	CSelfDllInfo::SetBindDllHandle();

	// 絶対に２文字は必要
	if (_tcslen(sz_full_var_name) < 2) {
		return 0;
	}

	char utf8_buffer[4096] = "";

	// 最初の記号は除いた文字。チェックしても無駄なのでチェックしない。
	const TCHAR *sz_var_name = sz_full_var_name + 1;

	int ret = module->PerlEzEvalString(
		module->engine,
		string("${" + utf16_to_utf8(sz_var_name) + "};").data(),
		utf8_buffer,
		_countof(utf8_buffer)
		);

	// 何かエラーっぽい文字列が入っているぞ…
	if (module->IsReturnBufferSeemsEvalError(utf8_buffer)) {
		OutputDebugStream(utf8_to_utf16(utf8_buffer));
	}

	// 成功
	if (ret == plezNoError) {
		// 数字→数値の変換
		try {
			wstring strnum = utf8_to_utf16(utf8_buffer);
			return (intHM_t)std::stoll(strnum);
		}
		catch (...) {

		}
	}
	OutputDebugErrMsg();
	// 失敗したらトレース枠に
	OutputDebugStream(utf8_to_utf16(utf8_buffer));

	// 失敗したら0
	return 0;
}

MACRO_DLL intHM_t SetNumVar(const TCHAR *sz_full_var_name, intHM_t value) {
	if (CreateScope() == 0)
	{
		return 0;
	}

	// DoStringされる度にdllのBindの在り方を確認更新する。
	CSelfDllInfo::SetBindDllHandle();

	// 絶対に２文字は必要
	if (_tcslen(sz_full_var_name) < 2) {
		return 0;
	}

	char utf8_buffer[4096] = "";

	// まずは関数を定義
	module->PerlEzEvalString(
		module->engine,
		string("sub _____TmpSetVar { my ($simbol, $value) = @_; ${$simbol} = $value; }").data(),
		utf8_buffer,
		_countof(utf8_buffer)
		);

	// クリア
	utf8_buffer[0] = NULL;

	// 最初の記号は除いた文字。チェックしても無駄なのでチェックしない。
	const TCHAR *sz_var_name = sz_full_var_name + 1;

	int ret = module->PerlEzCall(
		module->engine,
		"_____TmpSetVar",
		utf8_buffer,
		_countof(utf8_buffer),
		"si",
		utf16_to_utf8(sz_var_name).data(),
		value
		);

	// 何かエラーっぽい文字列が入っているぞ…
	if (module->IsReturnBufferSeemsEvalError(utf8_buffer)) {
		OutputDebugStream(utf8_to_utf16(utf8_buffer));
	}

	// 成功
	if (ret == plezNoError) {
		return 1;
	}
	OutputDebugErrMsg();
	// 失敗したらトレース枠に
	OutputDebugStream(utf8_to_utf16(utf8_buffer));

	// 失敗したら0
	return 0;
}

// 秀丸のキャッシュのため
static wstring strvars;
MACRO_DLL const TCHAR * GetStrVar(const TCHAR *sz_full_var_name) {
	// クリア必須
	strvars.clear();

	if (CreateScope() == 0)
	{
		return 0;
	}

	// DoStringされる度にdllのBindの在り方を確認更新する。
	CSelfDllInfo::SetBindDllHandle();

	// 絶対に２文字は必要
	if (_tcslen(sz_full_var_name) < 2) {
		return 0;
	}

	char utf8_buffer[4096] = "";

	// 最初の記号は除いた文字。チェックしても無駄なのでチェックしない。
	const TCHAR *sz_var_name = sz_full_var_name + 1;

	int ret = module->PerlEzEvalString(
		module->engine,
		string("${" + utf16_to_utf8(sz_var_name) + "};").data(),
		utf8_buffer,
		_countof(utf8_buffer)
		);

	// 何かエラーっぽい文字列が入っているぞ…
	if (module->IsReturnBufferSeemsEvalError(utf8_buffer)) {
		OutputDebugStream(utf8_to_utf16(utf8_buffer));
	}

	// 成功
	if (ret == plezNoError) {
		strvars = utf8_to_utf16(utf8_buffer);
		return strvars.data();
	}

	OutputDebugErrMsg();
	// 失敗したらトレース枠に
	OutputDebugStream(utf8_to_utf16(utf8_buffer));

	// 失敗したら""
	return L"";
}

MACRO_DLL intHM_t SetStrVar(const TCHAR *sz_full_var_name, const TCHAR *value) {
	if (CreateScope() == 0)
	{
		return 0;
	}

	// DoStringされる度にdllのBindの在り方を確認更新する。
	CSelfDllInfo::SetBindDllHandle();

	// 絶対に２文字は必要
	if (_tcslen(sz_full_var_name) < 2) {
		return 0;
	}

	char utf8_buffer[4096] = "";

	// まずは関数を定義
	module->PerlEzEvalString(
		module->engine,
		string("sub _____TmpSetVar { my ($simbol, $value) = @_; ${$simbol} = $value; }").data(),
		utf8_buffer,
		_countof(utf8_buffer)
		);

	// クリア
	utf8_buffer[0] = NULL;

	// 最初の記号は除いた文字。チェックしても無駄なのでチェックしない。
	const TCHAR *sz_var_name = sz_full_var_name + 1;

	int ret = module->PerlEzCall(
		module->engine,
		"_____TmpSetVar",
		utf8_buffer,
		_countof(utf8_buffer),
		"ss",
		utf16_to_utf8(sz_var_name).data(),
		utf16_to_utf8(value).data()
		);

	// 何かエラーっぽい文字列が入っているぞ…
	if (module->IsReturnBufferSeemsEvalError(utf8_buffer)) {
		OutputDebugStream(utf8_to_utf16(utf8_buffer));
	}

	// 成功
	if (ret == plezNoError) {
		return 1;
	}
	OutputDebugErrMsg();
	// 失敗したらトレース枠に
	OutputDebugStream(utf8_to_utf16(utf8_buffer));

	// 失敗したら0
	return 0;
}


MACRO_DLL intHM_t DoString(const TCHAR *szexpression) {
	if (CreateScope() == 0)
	{
		return 0;
	}

	// DoStringされる度にdllのBindの在り方を確認更新する。
	CSelfDllInfo::SetBindDllHandle();

	//-------------------------------------------------------------------------
	// ほとんどの場合、この「DoString」しか使われないハズなので、
	// この関数だけチェックしておく。
	//-------------------------------------------------------------------------
	auto rtn_type = (CHidemaruExeExport::DLLFUNCRETURN)CHidemaruExeExport::Hidemaru_GetDllFuncCalledType(0); // 0は返り値の型
	if (rtn_type == CHidemaruExeExport::DLLFUNCRETURN::CHAR_PTR || rtn_type == CHidemaruExeExport::DLLFUNCRETURN::WCHAR_PTR) {
		MessageBox(NULL, L"返り値の型が異なります。\ndllfuncstrではなく、dllfuncw文を利用してください。", L"返り値の型が異なります", MB_ICONERROR);
	}

	auto arg_type = (CHidemaruExeExport::DLLFUNCPARAM)CHidemaruExeExport::Hidemaru_GetDllFuncCalledType(1); // 1は1番目の引数
	if (arg_type != CHidemaruExeExport::DLLFUNCPARAM::WCHAR_PTR) {
		MessageBox(NULL, L"引数の型が異なります。\ndllfuncではなく、dllfuncw文を利用してください。", L"引数の型が異なります", MB_ICONERROR);
	}

	string utf8_expression = utf16_to_utf8(szexpression);
	char utf8_buffer[4096] = "";

	int ret = module->PerlEzEvalString(
		module->engine,
		utf8_expression.data(),
		utf8_buffer,
		_countof(utf8_buffer)
		);

	// 何かエラーっぽい文字列が入っているぞ…
	CPerlEzEngine::TEvalErrorInfo errInfo = module->GetEvalErrorInfo(szexpression, utf8_buffer);
	if (errInfo.HasError) {
		// まずはエラーメッセージ
		OutputDebugStream(utf8_to_utf16(utf8_buffer));

		if (errInfo.lineno >= 0) {
			// 次にエラー行のソースコード
			OutputDebugStream(errInfo.errorlinecode);
			// 最後にエラー位置に印文字を付ける
			OutputDebugStream(wstring(errInfo.column, L' ') + L"^");
		}
		return 0;
	}

	// 成功
	if (ret == plezNoError) {
		return 1;
	}

	OutputDebugErrMsg();

	// 失敗したら0
	return 0;
}


MACRO_DLL intHM_t DestroyScope() {
	if (module) {

		// 最後に「DestroyScope」というグローバル関数がPerl内に定義されていれば、それを実行する。
		char utf8_buffer[4096] = "";
		int ret = module->PerlEzCall(
			module->engine,
			"DestroyScope",
			utf8_buffer,
			_countof(utf8_buffer),
			""
			);

		// 何かエラーっぽい文字列が入っているぞ…
		if (module->IsReturnBufferSeemsEvalError(utf8_buffer)) {
			OutputDebugStream(utf8_to_utf16(utf8_buffer));
			OutputDebugStream(L"error in DestroyScope");
		}

		delete module;
		module = NULL;

	}

	TestDynamicVar.Clear();

	return TRUE;
}


MACRO_DLL intHM_t DllDetachFunc_After_Hm866() {
	return DestroyScope();
}