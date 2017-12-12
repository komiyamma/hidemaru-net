/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#include <windows.h>

#include "hidemaruexe_export.h"
#include "dllfunc_interface_internal.h"

using namespace std;

CDynamicValue TestDynamicVar;
void CDynamicValue::Clear() {
	this->num = 0;
	this->wstr = L"";
	this->type = TDynamicType::TypeUnknown;
}


// 秀丸の変数が文字列か数値かの判定用
MACRO_DLL intHM_t SetDynamicVar(const void* dynamic_value) {

	auto param_type = (CHidemaruExeExport::DLLFUNCPARAM)CHidemaruExeExport::Hidemaru_GetDllFuncCalledType(1);
	if (param_type == CHidemaruExeExport::DLLFUNCPARAM::WCHAR_PTR) {
		TestDynamicVar.wstr = wstring((wchar_t *)dynamic_value);
		TestDynamicVar.type = CDynamicValue::TDynamicType::TypeString;
		return 1;
	}
	else {
		TestDynamicVar.num = (intHM_t)dynamic_value;
		TestDynamicVar.type = CDynamicValue::TDynamicType::TypeInteger;
		return 1;
	}
}


intHM_t popnumvar = 0;
// スタックした変数を秀丸マクロから取り出す。内部処理用
MACRO_DLL intHM_t PopNumVar() {
	return popnumvar;
}

// 変数を秀丸マクロから取り出すためにスタック。内部処理用
MACRO_DLL intHM_t PushNumVar(intHM_t i_tmp_num) {
	popnumvar = i_tmp_num;
	return 1;
}

// スタックした変数を秀丸マクロから取り出す。内部処理用
static wstring popstrvar;
MACRO_DLL const TCHAR * PopStrVar() {
	return popstrvar.data();
}

// 変数を秀丸マクロから取り出すためにスタック。内部処理用
MACRO_DLL intHM_t PushStrVar(const TCHAR *sz_tmp_str) {
	popstrvar = sz_tmp_str;
	return 1;
}

