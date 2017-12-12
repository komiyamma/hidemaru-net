/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#pragma once

#include "dllfunc_interface.h"

#include <string>

using namespace std;

//========================================================================
/// 他のファイルから見ているためのextern。実際は.cppの方をみてね
//========================================================================
MACRO_DLL const TCHAR * PopStrVar();
MACRO_DLL intHM_t PushStrVar(const TCHAR *sz_tmp_str);
MACRO_DLL intHM_t PopNumVar();
MACRO_DLL intHM_t PushNumVar(intHM_t i_tmp_num);


//========================================================================
// 「数値か文字列か不明な秀丸マクロ変数」を「秀丸マクロからdllへ」と渡しし、
// Hidemaru_GetDllFuncCalledType を使うことで、振り分ける。シングルトン変数
struct CDynamicValue {
	// 数値か文字列かどちらか
	intHM_t num;
	wstring wstr;

	enum TDynamicType { TypeUnknown = 0, TypeInteger = 1, TypeString = 2 };
	TDynamicType type;

	void Clear();
};

extern CDynamicValue TestDynamicVar;


