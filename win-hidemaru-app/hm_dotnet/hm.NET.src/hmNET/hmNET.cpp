/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the MIT License
 */

#include <windows.h>
#include <string>

#include "string_converter.h"
#include "hidemaruexe_export.h"
#include "hmNET.h"
#include "hmNETStatlcLib.h"

#include "output_debugstream.h"

using namespace std;
using namespace System;
using namespace System::Reflection;
using namespace System::Collections::Generic;

// 上の手動のBindDllHandleを自動で行う。秀丸8.66以上
// １回だけ実行すれば良いわけではない。dllが読み込まれている間にもdll値が変わってしまうかもしれないため。(将来の実装では)
// よって、CallMethodの時を契機に更新する。
static bool BindDllHandle() {
	// 秀丸8.66以上
	if (Hidemaru_GetDllFuncCalledType) {
		int dll = Hidemaru_GetDllFuncCalledType(-1); // 自分のdllの呼ばれ方をチェック
		INETStaticLib::BindDllHandle((IntPtr)dll);
		return true;
	}
	return false;
}


// 秀丸の変数が文字列か数値かの判定用
MACRO_DLL intHM_t SetTmpVar(const void* dynamic_value) {
	int param_type = Hidemaru_GetDllFuncCalledType(1); // 1番目の引数の型。
	if (param_type == DLLFUNCPARAM_WCHAR_PTR) {
		return (intHM_t)INETStaticLib::SetTmpVar(gcnew String((wchar_t *)dynamic_value));
	}
	else {
		return (intHM_t)INETStaticLib::SetTmpVar((intHM_t)dynamic_value);
	}
}

MACRO_DLL intHM_t PopNumVar() {
	intHM_t num = (intHM_t)INETStaticLib::PopTmpVar();
	return num;
}

static wstring strvarsopop;
MACRO_DLL const TCHAR * PopStrVar() {
	auto var = (String ^)INETStaticLib::PopTmpVar();
	strvarsopop = String_to_wstring(var);
	return strvarsopop.data();
}

void TraceMethodInfo(String^ assm_path, String^ class_name, String^ method_name) {
	System::Diagnostics::Trace::WriteLine("アセンブリパス   :" + assm_path);
	System::Diagnostics::Trace::WriteLine("名前空間.クラス名:" + class_name);
	System::Diagnostics::Trace::WriteLine("メソッド名       :" + method_name);
}

void TraceExceptionInfo(Exception^ e) {
	System::Diagnostics::Trace::WriteLine(e->GetType());
	System::Diagnostics::Trace::WriteLine(e->Message);
	System::Diagnostics::Trace::WriteLine(e->StackTrace);
}


Object^ SubCallMethod(String^ assm_path, String^ class_name, String^ method_name, List<Object^>^ args, bool isDetathFuncMode = false) {
	try {
		Assembly^ assm = nullptr;
		Type^ t = nullptr;
		if (assm_path->Length > 0) {
			assm = Assembly::LoadFile(assm_path);
			if (assm == nullptr) {
				System::Diagnostics::Trace::WriteLine("ロード出来ない");
			}
			else {
				// System::Diagnostics::Trace::WriteLine(assm->FullName);
			}
			int i = 0;
			for each(Type^ t2 in assm->GetExportedTypes() ) {
				if (t2->ToString() == class_name) {
					t = assm->GetType(class_name);
				}
			}
		}
		else {
			t = Type::GetType(class_name);
		}
		if (t == nullptr) {
			System::Diagnostics::Trace::WriteLine("MissingMethodException(クラスもしくはメソッドを見つけることが出来ない):");
			TraceMethodInfo( assm_path, class_name, method_name);
			return nullptr;
		}
		MethodInfo^ m = t->GetMethod(method_name);
		Object^ o = nullptr;
		try {
			// デタッチ関数の時に、引数が無いパターンを許す
			if (isDetathFuncMode) {
				auto paramInfo = m->GetParameters();
				// 受け側の破棄メソッドの「引数が無い」ならば
				if (paramInfo->Length == 0) {
					o = m->Invoke(nullptr, nullptr);
					// System::Diagnostics::Trace::WriteLine("引数無し");
				}
				// 引数があるならば、一応その値を渡す。とは言っても基本的にアセンブリなのでアプリ終了時にしかこれは実行されないハズであるが…
				else {
					o = m->Invoke(nullptr, args->ToArray());
					// System::Diagnostics::Trace::WriteLine("引数有り");
				}
			}
			else {
				o = m->Invoke(nullptr, args->ToArray());
			}
		}
		catch (Exception^) {
			System::Diagnostics::Trace::WriteLine("指定のメソッドの実行時、例外が発生しました。");
			throw;
		}
		return o;
	}
	catch (Exception ^e) {
		System::Diagnostics::Trace::WriteLine("指定のアセンブリやメソッドを特定する前に、例外が発生しました。");
		TraceMethodInfo(assm_path, class_name, method_name);
		TraceExceptionInfo(e);
	}


	return nullptr;
}

intHM_t ConvertObjectToIntPtr(Object^ o) {
	// Boolean型であれば、True:1 Flase:0にマッピングする
	if (o->GetType()->Name == "Boolean")
	{
		if ((Boolean)o == true)
		{
			return (intHM_t)1;
		}
		else
		{
			return (intHM_t)0;
		}
	}

	// 32bit
	if (IntPtr::Size == 4)
	{
		// まずは整数でトライ
		Int32 itmp = 0;
		bool success = Int32::TryParse(o->ToString(), itmp);

		if (success == true)
		{
			return (intHM_t)itmp;
		}

		else
		{
			// 次に浮動少数でトライ
			Double dtmp = 0;
			success = Double::TryParse(o->ToString(), dtmp);
			if (success)
			{
				return (intHM_t)(Int32)Math::Floor(dtmp);
			}
			else
			{
				return (intHM_t)0;
			}
		}
	}

	// 64bit
	else
	{
		// まずは整数でトライ
		Int64 itmp = 0;
		bool success = Int64::TryParse(o->ToString(), itmp);

		if (success == true)
		{
			return (intHM_t)itmp;
		}

		else
		{
			// 次に浮動少数でトライ
			Double dtmp = 0;
			success = Double::TryParse(o->ToString(), dtmp);
			if (success)
			{
				return (intHM_t)(Int64)Math::Floor(dtmp);
			}
			else
			{
				return (intHM_t)0;
			}
		}
	}
	return (intHM_t)0;
}

static wstring strcallmethod;
MACRO_DLL intHM_t CallMethod(const wchar_t* assm_path, const wchar_t* class_name, wchar_t* method_name, void *arg0, void *arg1, void *arg2, void *arg3) {

	// 自分自身のCallMethodを別から再度呼ぶと値が崩れるので、いち早く取得
	int rty = Hidemaru_GetDllFuncCalledType(0);
	int pt0 = Hidemaru_GetDllFuncCalledType(4); // 引数４番目
	int pt1 = Hidemaru_GetDllFuncCalledType(5); // 引数５番目
	int pt2 = Hidemaru_GetDllFuncCalledType(6); // 引数６番目
	int pt3 = Hidemaru_GetDllFuncCalledType(7); // 引数７番目

	try {

		strcallmethod.clear();

		BindDllHandle();

		List<Object^>^ args = gcnew List<Object^>();
		int pt = 0;

		for (int i = 4; true; i++) {

			void *arg = nullptr;
			int pt = 0;
			switch (i) {
			case 4: {
				arg = arg0;
				pt = pt0;
				break;
			}
			case 5: {
				arg = arg1;
				pt = pt1;
				break;
			}
			case 6: {
				arg = arg2;
				pt = pt2;
				break;
			}
			case 7: {
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
				args->Add((IntPtr)arg);
			}
			else if (pt == DLLFUNCPARAM_WCHAR_PTR) {
				args->Add(gcnew String((wchar_t *)arg));
			}
		}

		INETStaticLib::CallMethod(L"init");

		Object^ o = SubCallMethod(wstring_to_String(assm_path), wstring_to_String(class_name), wstring_to_String(method_name), args);

		GC::Collect();

		if (rty == DLLFUNCRETURN_INT) {
			// System::Diagnostics::Trace::WriteLine("数値リターン");
			if (o == nullptr) {
				return (intHM_t)0;
			}
			try {
				intHM_t rtn = ConvertObjectToIntPtr(o);
				return rtn;
			}
			catch (Exception^ e) {
				System::Diagnostics::Trace::WriteLine(e->GetType());
				System::Diagnostics::Trace::WriteLine(e->Message);
			}
			return (intHM_t)0;

		}
		else if (rty == DLLFUNCRETURN_WCHAR_PTR) {
			// System::Diagnostics::Trace::WriteLine("文字列リターン");
			strcallmethod = String_to_wstring(o->ToString());
			return (intHM_t)strcallmethod.data();

		}

		return false;

	}
	catch (Exception^ ex) {
		TraceExceptionInfo(ex);
		if (rty == DLLFUNCRETURN_INT) {
			return (intHM_t)0;
		}
		else if (rty == DLLFUNCRETURN_WCHAR_PTR) {
			// System::Diagnostics::Trace::WriteLine("文字列リターン");
			strcallmethod = L"";
			return (intHM_t)strcallmethod.data();

		}

		return false;
	}
}

struct DETATH_FUNC {
	wstring assm_path;
	wstring class_name;
	wstring method_name;
};

static vector<DETATH_FUNC> detach_func_list;
MACRO_DLL intHM_t SetDetachMethod(const wchar_t* assm_path, const wchar_t* class_name, wchar_t* method_name) {
	bool is_exist = false;
	for each(auto v in detach_func_list) {
		if (v.assm_path == assm_path && v.class_name == class_name && v.method_name == method_name) {
			is_exist = true;
		}
	}
	if (!is_exist) {
		DETATH_FUNC f = { assm_path, class_name, method_name };
		detach_func_list.push_back(f);
	}
	return true;
}

MACRO_DLL intHM_t DetachScope(intHM_t n) {
	strcallmethod.clear();
	BindDllHandle();

	List<Object^>^ args = gcnew List<Object^>();
	args->Add((IntPtr)n); // 終了時のパラメータを付け足し
	for each(auto v in detach_func_list) {
		SubCallMethod(wstring_to_String(v.assm_path), wstring_to_String(v.class_name), wstring_to_String(v.method_name), args, true);
	}

	intHM_t ret = (intHM_t)INETStaticLib::DetachScope(System::IntPtr(n));

	GC::Collect();
	
	return ret;
}

MACRO_DLL intHM_t DllDetachFunc_After_Hm866(intHM_t n) {

	intHM_t ret = DetachScope(n);

	// v8.77未満だと、nは常に0
	if (n == 0) {
		// OutputDebugStream(L"v8.66未満\n");
	}
	else if (n == 1) {
		MessageBox(NULL, L"hm.NET.dllをマクロ制御下で解放をしてはいけません。\n"
			L"(freedllによる解放エラー)", L"エラー", MB_ICONERROR);
		// freedll
	}
	else if (n == 2) {
		MessageBox(NULL, L"hm.NET.dllをマクロ制御下で解放をしてはいけません。\n"
			L"「loaddll文」ではなく、「loaddll関数」を利用してください。\n"
			L"(新たなloaddll文による、hm.NET.dllの暗黙的解放エラー)", L"エラー", MB_ICONERROR);
		// loaddll文による入れ替え
	}
	else if (n == 3) {
		// OutputDebugStream(L"プロセス終了時\n");
		// プロセス終了時
	}
	else {
		// 未知の数
	}

	return ret;
}
