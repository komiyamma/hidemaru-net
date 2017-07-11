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



Object^ SubCallMethod(wstring assm_path, wstring class_name, wstring method_name, List<Object^>^ args) {
	try {
		Assembly^ assm = nullptr;
		Type^ t = nullptr;
		if (assm_path.size() > 0) {
			System::Diagnostics::Trace::WriteLine(gcnew String(assm_path.data()));
			assm = Assembly::LoadFile(gcnew String(assm_path.data()));
			if (assm == nullptr) {
				System::Diagnostics::Trace::WriteLine("ロード出来ない");
			}
			else {
				System::Diagnostics::Trace::WriteLine(assm->FullName);
			}
			int i = 0;
			String^ target_class_name = gcnew String(class_name.data());
			for each(Type^ t2 in assm->GetExportedTypes() ) {
				System::Diagnostics::Trace::Write(i);
				if (t2->ToString() == target_class_name) {
					t = assm->GetType(target_class_name);
				}
			}
		}
		else {
			t = Type::GetType(gcnew String(class_name.data()));
			System::Diagnostics::Trace::WriteLine(t->ToString());
		}
		if (t == nullptr) {
			System::Diagnostics::Trace::WriteLine("MissingMethodException(クラスもしくはメソッドを見つけることが出来ない):");
			System::Diagnostics::Trace::WriteLine("アセンブリパス   :" + gcnew String(assm_path.data()));
			System::Diagnostics::Trace::WriteLine("名前空間.クラス名:" + gcnew String(class_name.data()));
			System::Diagnostics::Trace::WriteLine("メソッド名       :" + gcnew String(method_name.data()));
		}
		MethodInfo^ m = t->GetMethod(gcnew String(method_name.data()));
		// cli::array<ParameterInfo^>^ prms = m->GetParameters();
		Object^ o = nullptr;
		// System::Diagnostics::Trace::WriteLine(prms->Length);

		o = m->Invoke(nullptr, args->ToArray());
		System::Diagnostics::Trace::WriteLine(o);
		return o;
	}
	catch (Exception ^e) {
		System::Diagnostics::Trace::WriteLine(e->GetType());
		System::Diagnostics::Trace::WriteLine(e->Message);
		System::Diagnostics::Trace::WriteLine(e->StackTrace);
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
MACRO_DLL intHM_t CallMethod(const wchar_t* assm_path, const wchar_t* class_name, wchar_t* method_name, void *arg0, void *arg1, void *arg2) {

	// 自分自身のCallMethodを別から再度呼ぶと値が崩れるので、いち早く取得
	int rt = Hidemaru_GetDllFuncCalledType(0);
	int pt0 = Hidemaru_GetDllFuncCalledType(4); // 引数４番目
	int pt1 = Hidemaru_GetDllFuncCalledType(5); // 引数５番目
	int pt2 = Hidemaru_GetDllFuncCalledType(6); // 引数６番目

	strcallmethod.clear();

	BindDllHandle();

	List<Object^>^ args = gcnew List<Object^>();
	int pt = 0;

	for (int i=4; true; i++) {

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

	Object^ o = SubCallMethod(assm_path, class_name, method_name, args);

	if (rt == DLLFUNCRETURN_INT) {
		System::Diagnostics::Trace::WriteLine("数値リターン");
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

	} else if (rt == DLLFUNCRETURN_WCHAR_PTR) {
		System::Diagnostics::Trace::WriteLine("文字列リターン");
		strcallmethod = String_to_wstring(o->ToString());
		return (intHM_t)strcallmethod.data();

	}

	GC::Collect();
	return true;
}

struct FINALIZER {
	wstring assm_path;
	wstring class_name;
	wstring method_name;
};

static vector<FINALIZER> finalizer_list;
MACRO_DLL intHM_t SetDispose(const wchar_t* assm_path, const wchar_t* class_name, wchar_t* method_name) {
	bool is_exist = false;
	for each(auto v in finalizer_list) {
		if (v.assm_path == assm_path && v.class_name == class_name && v.method_name == method_name) {
			is_exist = true;
		}
	}
	if (!is_exist) {
		FINALIZER f = { assm_path, class_name, method_name };
		finalizer_list.push_back(f);
	}
	return true;
}

MACRO_DLL intHM_t DestroyScope() {
	strcallmethod.clear();
	BindDllHandle();

	List<Object^>^ args = gcnew List<Object^>();
	for each(auto v in finalizer_list) {
		System::Diagnostics::Trace::WriteLine("DestroyScope");
		System::Diagnostics::Trace::WriteLine(gcnew String(v.assm_path.c_str()));
		System::Diagnostics::Trace::WriteLine(gcnew String(v.class_name.c_str()));
		System::Diagnostics::Trace::WriteLine(gcnew String(v.method_name.c_str()));
		SubCallMethod(v.assm_path, v.class_name, v.method_name, args);
	}

	GC::Collect();
	intHM_t ret = (intHM_t)INETStaticLib::DestroyScope();
	return ret;
}

MACRO_DLL intHM_t DllDetachFunc_After_Hm866() {
	return DestroyScope();
}