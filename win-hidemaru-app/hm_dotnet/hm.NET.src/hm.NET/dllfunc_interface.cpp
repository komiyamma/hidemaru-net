#include <stdio.h>
#include <windows.h>
#include <vector>

#include "convert_string.h"
#include "hidemaruexe_export.h"
#include "self_dll_info.h"

#include "dllfunc_interface.h"
#include "dllfunc_interface_internal.h"
#include "output_debugstream.h"
#include "HmPublicClass.h"

using namespace std;
using namespace System;


using namespace System;
using namespace System::Reflection;
static Assembly^ CurrentDomain_AssemblyResolve(Object^ sender, ResolveEventArgs^ args) {
	try
	{
		auto requestingAssembly = args->RequestingAssembly;
		auto requestedAssembly = gcnew AssemblyName(args->Name);

		// このdll自体を置いているフォルダに読み込み対象のアセンブリがあるかもしれない。
		String^ self_full_path = System::Reflection::Assembly::GetExecutingAssembly()->Location;
		String^ self_dir = System::IO::Path::GetDirectoryName(self_full_path);

		auto targetfullpath = self_dir + R"(\)" + requestedAssembly->Name + ".dll";

		if (System::IO::File::Exists(targetfullpath))
		{
			return Assembly::LoadFile(targetfullpath);
		}

		// そのようなフルパスが指定されている場合(フルパスを指定した書き方)
		targetfullpath = requestedAssembly->Name;
		if (System::IO::File::Exists(targetfullpath))
		{
			return Assembly::LoadFile(targetfullpath);
		}
	}
	catch (...)
	{
		return nullptr;
	}
	return nullptr;
}


struct __declspec(dllexport) DllAssemblyResolver {
	DllAssemblyResolver() {
		AppDomain::CurrentDomain->AssemblyResolve += gcnew System::ResolveEventHandler(&CurrentDomain_AssemblyResolve);
	}

	~DllAssemblyResolver() {
		AppDomain::CurrentDomain->AssemblyResolve -= gcnew System::ResolveEventHandler(&CurrentDomain_AssemblyResolve);
	}
};



static int CreateScope() {
	return TRUE;
}

using namespace System;
using namespace System::Reflection;

void SubCallMethod(wstring assm_path, wstring class_name, wstring method_name) {
	try {
		OutputDebugStream(L" 1 ");
		Assembly^ assm = nullptr;
		Type^ t = nullptr;
		if (assm_path.size() > 0) {
			assm = Assembly::LoadFile(gcnew String(assm_path.data()));
			t = assm->GetType(gcnew String(class_name.data()));
		}
		else {
			t = Type::GetType(gcnew String(class_name.data()));
		}
		OutputDebugStream(L" 2 ");
		MethodInfo^ m = t->GetMethod(gcnew String(method_name.data()));
		cli::array<ParameterInfo^>^ prms = m->GetParameters();
		Object^ o = nullptr;
		System::Diagnostics::Trace::WriteLine(prms->Length);

		o = m->Invoke(m->Name, nullptr);
	}
	catch (Exception ^e) {
		System::Diagnostics::Trace::WriteLine(e->Message);
		OutputDebugStream(L" 7 ");
	}
	OutputDebugStream(L" 8 ");
}

#pragma unmanaged
MACRO_DLL intHM_t CallMethod(const wchar_t* assm_path, const wchar_t* class_name, wchar_t* method_name) {
	if (CreateScope() == 0)
	{
		return 0;
	}

	CSelfDllInfo::SetBindDllHandle();

	auto rtn_type = (CHidemaruExeExport::DLLFUNCRETURN)CHidemaruExeExport::Hidemaru_GetDllFuncCalledType(0); // 0は返り値の型

	auto nAssemblyNameType = (CHidemaruExeExport::DLLFUNCPARAM)CHidemaruExeExport::Hidemaru_GetDllFuncCalledType(1); // 1は1番目の引数
	if (nAssemblyNameType != CHidemaruExeExport::DLLFUNCPARAM::WCHAR_PTR) {
		MessageBox(NULL, L"第１引数の型が異なります。\ndllfuncではなく、dllfuncw, もしくはdllfuncwstr 文を利用してください。", L"第１引数の型が異なります", MB_ICONERROR);
		return 0;
	}

	auto nClassNameType = (CHidemaruExeExport::DLLFUNCPARAM)CHidemaruExeExport::Hidemaru_GetDllFuncCalledType(2); // 2は1番目の引数
	if (nClassNameType != CHidemaruExeExport::DLLFUNCPARAM::WCHAR_PTR) {
		MessageBox(NULL, L"第２引数の型が異なります。\ndllfuncではなく、dllfuncw, もしくはdllfuncwstr 文を利用してください。", L"第２引数の型が異なります", MB_ICONERROR);
		return 0;
	}

	auto nMethodName = (CHidemaruExeExport::DLLFUNCPARAM)CHidemaruExeExport::Hidemaru_GetDllFuncCalledType(3); // 3は1番目の引数
	if (nMethodName != CHidemaruExeExport::DLLFUNCPARAM::WCHAR_PTR) {
		MessageBox(NULL, L"第２引数の型が異なります。\ndllfuncではなく、dllfuncw, もしくはdllfuncwstr 文を利用してください。", L"第２引数の型が異なります", MB_ICONERROR);
		return 0;
	}

	SubCallMethod(assm_path, class_name, method_name);

	return true;
}
#pragma managed


MACRO_DLL intHM_t DestroyScope() {
	TestDynamicVar.Clear();

	return TRUE;
}


MACRO_DLL intHM_t DllDetachFunc_After_Hm866() {
	return DestroyScope();
}