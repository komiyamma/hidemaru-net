//------------------------------------------------------------------------------------------------
#include <windows.h>
#include "string_converter.h"

using namespace System;
using namespace System::Reflection;
static Assembly^ CurrentDomain_AssemblyResolve(Object^ sender, ResolveEventArgs^ args) {
	try
	{
		auto requestingAssembly = args->RequestingAssembly;
		auto requestedAssembly = gcnew AssemblyName(args->Name);

		// IronPython.dllが求められており、なおかつ、Public Key Tokenがちゃんと設定されている。
		if (requestedAssembly->Name->ToUpper() == "IRONPYTHON" && requestedAssembly->GetPublicKeyToken()->ToString() != String::Empty ) {
			System::Diagnostics::Trace::WriteLine(args->Name);
			wchar_t message[256] = L"";

			wsprintf
			(
				message,
				L"http://IronPython.net/" L"から、\n"
				L"IronPython %d.%d.%dを「.msiインストーラーを使って」インストールしてください。\n"
				L"(※注意 : バージョンは正確に、「%d.%d.%d」の必要性があります)",
				requestedAssembly->Version->Major, requestedAssembly->Version->Minor, requestedAssembly->Version->Build,
				requestedAssembly->Version->Major, requestedAssembly->Version->Minor, requestedAssembly->Version->Build
			);

			MessageBox(NULL, message, L"IronPython.dllが見つからない!!", MB_ICONERROR);
		}
		else {

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



//----------ここだけネイティブ
#pragma unmanaged 
// ほかのマネージドコードより先に実行させるため。
// 先にここを定義して実行。アンマネージド
DllAssemblyResolver asmresolver;
#pragma managed
//----------ここだけネイティブ

//------------------------------------------------------------------------------------------------
