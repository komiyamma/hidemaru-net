﻿/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#include <stdio.h>
#include <windows.h>

#include "convert_string.h"
#include "java_vm_engine.h"
#include "hidemaruexe_export.h"
#include "self_dll_info.h"

#include "dllfunc_interface.h"
#include "dllfunc_interface_internal.h"
#include "output_debugstream.h"

using namespace std;



static int CreateScope() {

	// すでに生成済みである。
	if (CJavaVMEngine::IsValid()) {
		OutputDebugStream(L"JavaVMは起動済み。流用。");
		return TRUE;
	}

	OutputDebugStream(L"JavaVMは未起動。起動を試みる。");
	CJavaVMEngine::CreateVM();
	if (!CJavaVMEngine::IsValid()) {
		return FALSE;
	}

	return TRUE;
}


static wstring strcallmethod;
MACRO_DLL intHM_t CallMethod(const TCHAR *class_name, TCHAR *method_name, void *arg0, void *arg1, void *arg2, void *arg3) {
	if (CreateScope() == 0)
	{
		return 0;
	}

	// DoStringされる度にdllのBindの在り方を確認更新する。
	CSelfDllInfo::SetBindDllHandle();

	strcallmethod.clear();

	auto rtn_type = (CHidemaruExeExport::DLLFUNCRETURN)CHidemaruExeExport::Hidemaru_GetDllFuncCalledType(0); // 0は返り値の型
	if (rtn_type != CHidemaruExeExport::DLLFUNCRETURN::INT && rtn_type != CHidemaruExeExport::DLLFUNCRETURN::WCHAR_PTR) {
		MessageBox(NULL, L"返り値の型が異なります。\n「dllfuncstrw」、もしくは「dllfuncw」文を利用してください。\n「w」が付きます。", L"返り値の型が異なります", MB_ICONERROR);
		return 0;
	}

	auto arg1_type = (CHidemaruExeExport::DLLFUNCPARAM)CHidemaruExeExport::Hidemaru_GetDllFuncCalledType(1); // 1は1番目の引数
	if (arg1_type != CHidemaruExeExport::DLLFUNCPARAM::WCHAR_PTR) {
		MessageBox(NULL, L"第１引数の型が異なります。\ndllfuncではなく、dllfuncw文を利用してください。", L"第１引数の型が異なります", MB_ICONERROR);
		return 0;
	}

	auto arg2_type = (CHidemaruExeExport::DLLFUNCPARAM)CHidemaruExeExport::Hidemaru_GetDllFuncCalledType(2); // 2は2番目の引数
	if (arg2_type != CHidemaruExeExport::DLLFUNCPARAM::WCHAR_PTR) {
		MessageBox(NULL, L"第２引数の型が異なります。", L"第２引数の型が異なります", MB_ICONERROR);
		return 0;
	}

	int pt0 = CHidemaruExeExport::Hidemaru_GetDllFuncCalledType(3); // 引数３番目
	int pt1 = CHidemaruExeExport::Hidemaru_GetDllFuncCalledType(4); // 引数４番目
	int pt2 = CHidemaruExeExport::Hidemaru_GetDllFuncCalledType(5); // 引数５番目
	int pt3 = CHidemaruExeExport::Hidemaru_GetDllFuncCalledType(6); // 引数６番目

	string method_args_typedef_string = "";
	string method_args_declare_string = "";

	int pt = 0;
	method_args_typedef_string += "(";
	method_args_declare_string += "(";

	for (int i = 3; true; i++) {

		void *arg = nullptr;
		int pt = 0;
		switch (i) {
		case 3: {
			arg = arg0;
			pt = pt0;
			break;
		}
		case 4: {
			arg = arg1;
			pt = pt1;
			break;
		}
		case 5: {
			arg = arg2;
			pt = pt2;
			break;
		}
		case 6: {
			arg = arg3;
			pt = pt3;
			break;
		}
		}

		// arg0のチェック
		if (pt == DLLFUNCPARAM_NOPARAM) {
			break;
		}
		else if (pt == DLLFUNCPARAM_INT) {
			method_args_typedef_string += "J";
			method_args_declare_string += "long, ";
		}
		else if (pt == DLLFUNCPARAM_WCHAR_PTR) {
			method_args_typedef_string += "Ljava/lang/String;";
			method_args_declare_string += "String, ";
		}
	}
	method_args_typedef_string += ")V";
	method_args_declare_string += ")";

	// このプロセスで最初の１回
	if (!CJavaVMEngine::HmCalled) {
		bool success = CJavaVMEngine::CallStaticEntryMethod(L"hidemaru/Hm", L"_Init");
		if (!success) {
			if (rtn_type == CHidemaruExeExport::DLLFUNCRETURN::INT) {
				return (intHM_t)0;

			}
			else if (rtn_type == CHidemaruExeExport::DLLFUNCRETURN::WCHAR_PTR) {
				strcallmethod = wstring(L"aaa");
				return (intHM_t)strcallmethod.data();

			}
			MessageBox(NULL, L"返り値の型が異なります。\n「dllfuncstrw」、もしくは「dllfuncw」文を利用してください。\n「w」が付きます。", L"返り値の型が異なります", MB_ICONERROR);
			return (intHM_t)0;
		}
		CJavaVMEngine::HmCalled = true;
	}

	// JavaVMがクラスを探し出すパスをダイナミックに追加する
	CJavaVMEngine::CallStaticEntryMethod(L"hidemaru/Hm", L"_AddClassPath");

	// クラスは、「.」区切りで記載しているなら、「/」区切りに直す
	wstring wstr_class_name = class_name;
	std::replace(wstr_class_name.begin(), wstr_class_name.end(), '.', '/');

	bool success = CJavaVMEngine::CallStaticEntryMethod(wstr_class_name.c_str(), method_name, method_args_typedef_string, method_args_declare_string);
	return success;
}


MACRO_DLL intHM_t DestroyScope() {
	CJavaVMEngine::DestroyVM();

	TestDynamicVar.Clear();

	return TRUE;
}


MACRO_DLL intHM_t DllDetachFunc_After_Hm866() {
	return DestroyScope();
}