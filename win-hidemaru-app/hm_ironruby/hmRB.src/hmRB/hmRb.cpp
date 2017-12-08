/* 
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */


#include <windows.h>
#include <string>

#include "string_converter.h"
#include "hidemaruexe_export.h"
#include "hmRb.h"
#include "hmRbStatlcLib.h"


using namespace std;
using namespace System;



// 上の手動のBindDllHandleを自動で行う。秀丸8.66以上
// １回だけ実行すれば良いわけではない。dllが読み込まれている間にもdll値が変わってしまうかもしれないため。(将来の実装では)
// よって、DoStringとDoFileの時を契機に更新する。
static bool BindDllHandle() {
	// 秀丸8.66以上
	if (Hidemaru_GetDllFuncCalledType) {
		int dll = Hidemaru_GetDllFuncCalledType(-1); // 自分のdllの呼ばれ方をチェック
		IRbStaticLib::BindDllHandle((IntPtr)dll);
		return true;
	}
	return false;
}


MACRO_DLL intHM_t SetCodePage(intHM_t cp) {
	IRbStaticLib::SetCodePage((IntPtr)cp);
	return TRUE;
}

// 秀丸の変数が文字列か数値かの判定用
MACRO_DLL intHM_t SetTmpVar(const void* dynamic_value) {
	int param_type = Hidemaru_GetDllFuncCalledType(1); // 1番目の引数の型。
	if (param_type == DLLFUNCPARAM_WCHAR_PTR) {
		return (intHM_t)IRbStaticLib::SetTmpVar(gcnew String((wchar_t *)dynamic_value));
	}
	else {
		return (intHM_t)IRbStaticLib::SetTmpVar((intHM_t)dynamic_value);
	}
}

MACRO_DLL intHM_t PopNumVar() {
	intHM_t num = (intHM_t)IRbStaticLib::PopTmpVar();
	return num;
}

static wstring strvarsopop;
MACRO_DLL const TCHAR * PopStrVar() {
	auto var = (String ^)IRbStaticLib::PopTmpVar();
	strvarsopop = String_to_tstring(var);
	return strvarsopop.data();
}

//------------------------------------------------------------------------------------
MACRO_DLL intHM_t GetNumVar(const TCHAR *sz_var_name) {
	BindDllHandle();

	return (intHM_t)IRbStaticLib::GetNumVar(gcnew String(sz_var_name));
}

MACRO_DLL intHM_t SetNumVar(const TCHAR *sz_var_name, intHM_t value) {
	BindDllHandle();

	return (intHM_t)IRbStaticLib::SetNumVar(gcnew String(sz_var_name), (IntPtr)value);
}

// 秀丸のキャッシュのため
static wstring strvars;
MACRO_DLL const TCHAR * GetStrVar(const TCHAR *sz_var_name) {
	BindDllHandle();

	auto var = IRbStaticLib::GetStrVar(gcnew String(sz_var_name));
	strvars = String_to_tstring(var->ToString());
	return strvars.data();
}

MACRO_DLL intHM_t SetStrVar(const TCHAR *sz_var_name, const TCHAR *value) {
	BindDllHandle();

	return (intHM_t)IRbStaticLib::SetStrVar(gcnew String(sz_var_name), gcnew String(value));
}
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
MACRO_DLL intHM_t GetNumItemOfList(const TCHAR *sz_arr_name, const intHM_t index) {
	BindDllHandle();

	return (intHM_t)IRbStaticLib::GetNumItemOfList(gcnew String(sz_arr_name), (IntPtr)index);
}

MACRO_DLL intHM_t SetNumItemOfList(const TCHAR *sz_arr_name, const intHM_t index, const intHM_t value) {
	BindDllHandle();

	return (intHM_t)IRbStaticLib::SetNumItemOfList(gcnew String(sz_arr_name), (IntPtr)index, (IntPtr)value);
}

// 秀丸のキャッシュのため
static wstring strvarsoflist;
MACRO_DLL const TCHAR * GetStrItemOfList(const TCHAR *sz_arr_name, const intHM_t index) {
	BindDllHandle();

	auto var = IRbStaticLib::GetStrItemOfList(gcnew String(sz_arr_name), (IntPtr)index);
	strvarsoflist = String_to_tstring(var->ToString());
	return strvarsoflist.data();
}

MACRO_DLL intHM_t SetStrItemOfList(const TCHAR *sz_arr_name, const intHM_t index, const TCHAR *value) {
	BindDllHandle();

	return (intHM_t)IRbStaticLib::SetStrItemOfList(gcnew String(sz_arr_name), (IntPtr)index, gcnew String(value));
}
//------------------------------------------------------------------------------------



//------------------------------------------------------------------------------------
MACRO_DLL intHM_t GetNumItemOfDict(const TCHAR *sz_arr_name, const TCHAR *key) {
	BindDllHandle();

	return (intHM_t)IRbStaticLib::GetNumItemOfDict(gcnew String(sz_arr_name), gcnew String(key));
}

MACRO_DLL intHM_t SetNumItemOfDict(const TCHAR *sz_arr_name, const TCHAR *key, const intHM_t value) {
	BindDllHandle();

	return (intHM_t)IRbStaticLib::SetNumItemOfDict(gcnew String(sz_arr_name), gcnew String(key), (IntPtr)value);
}

// 秀丸のキャッシュのため

static wstring strvarsofdict;
MACRO_DLL const TCHAR * GetStrItemOfDict(const TCHAR *sz_arr_name, const TCHAR *key) {
	BindDllHandle();

	auto var = IRbStaticLib::GetStrItemOfDict(gcnew String(sz_arr_name), gcnew String(key));
	strvarsofdict = String_to_tstring(var->ToString());
	return strvarsofdict.data();
}

MACRO_DLL intHM_t SetStrItemOfDict(const TCHAR *sz_arr_name, const TCHAR *key, const TCHAR *value) {
	BindDllHandle();

	return (intHM_t)IRbStaticLib::SetStrItemOfDict(gcnew String(sz_arr_name), gcnew String(key), gcnew String(value));
}
//------------------------------------------------------------------------------------


MACRO_DLL intHM_t DoString(const TCHAR *szexpression) {
	BindDllHandle();

	// ここはよく間違えるのでここだけチェック。他は秀丸8.66以降ではほとんど利用しないので無視
	if (Hidemaru_GetDllFuncCalledType) {
		auto rtn_type = Hidemaru_GetDllFuncCalledType(0); // 0は返り値の型
		if (rtn_type == DLLFUNCRETURN_CHAR_PTR || rtn_type == DLLFUNCRETURN_WCHAR_PTR) {
			MessageBox(NULL, L"返り値の型が異なります。\ndllfuncstrではなく、dllfuncw文を利用してください。", L"返り値の型が異なります", MB_ICONERROR);
		}

		auto arg_type = Hidemaru_GetDllFuncCalledType(1); // 1は1番目の引数
		if (arg_type != DLLFUNCPARAM_WCHAR_PTR) {
			MessageBox(NULL, L"引数の型が異なります。\ndllfuncではなく、dllfuncw文を利用してください。", L"引数の型が異なります", MB_ICONERROR);
		}
	}

	return (intHM_t)IRbStaticLib::DoString(gcnew String(szexpression));
}

MACRO_DLL intHM_t DoFile(const TCHAR *szfilename) {
	BindDllHandle();

	// ここはよく間違えるのでここだけチェック。他は秀丸8.66以降ではほとんど利用しないので無視
	if (Hidemaru_GetDllFuncCalledType) {
		auto rtn_type = Hidemaru_GetDllFuncCalledType(0); // 0は返り値の型
		if (rtn_type == DLLFUNCRETURN_CHAR_PTR || rtn_type == DLLFUNCRETURN_WCHAR_PTR) {
			MessageBox(NULL, L"返り値の型が異なります。\ndllfuncstrではなく、dllfuncw文を利用してください。", L"返り値の型が異なります", MB_ICONERROR);
		}

		auto arg_type = Hidemaru_GetDllFuncCalledType(1); // 1は1番目の引数
		if (arg_type != DLLFUNCPARAM_WCHAR_PTR) {
			MessageBox(NULL, L"引数の型が異なります。\ndllfuncではなく、dllfuncw文を利用してください。", L"引数の型が異なります", MB_ICONERROR);
		}
	}

	return (intHM_t)IRbStaticLib::DoFile(gcnew String(szfilename));
}

MACRO_DLL intHM_t DestroyScope() {
	return (intHM_t)IRbStaticLib::DestroyScope();
}

MACRO_DLL intHM_t DllDetachFunc_After_Hm866() {
	return DestroyScope();
}