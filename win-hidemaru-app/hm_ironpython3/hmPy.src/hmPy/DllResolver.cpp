/* 
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

//------------------------------------------------------------------------------------------------
#include <windows.h>
#include "string_converter.h"
#include "resource.h"

using namespace System;
using namespace System::Reflection;
static Assembly^ CurrentDomain_AssemblyResolve(Object^ sender, ResolveEventArgs^ args) {

	try
	{
		auto requestingAssembly = args->RequestingAssembly;
		auto requestedAssembly = gcnew AssemblyName(args->Name);

		if (requestedAssembly->GetPublicKeyToken()->ToString() != String::Empty) {
			// IronPython.dllが求められており、なおかつ、Public Key Tokenがちゃんと設定されている。
			auto targetName = requestedAssembly->Name->ToUpper();

			if (targetName == "IRONPYTHON" || targetName == "IRONPYTHON.MODULES" || targetName == "MICROSOFT.DYNAMIC" || targetName == "MICROSOFT.SCRIPTING" || targetName == "MICROSOFT.SCRIPTING.METADATA") {
				System::Diagnostics::Trace::WriteLine(args->Name);


				wchar_t message[256] = L"";
				
				wsprintf
					(
					message,
					L"http://IronPython.net/" L"から、\n"
					L"対象のIronPython をインストールしてください。\n"
					L"「.msiインストーラー」版を利用してください。\n"
					L"%s",
					IRONPYTHON_VERSION
					);

				MessageBox(NULL, message, L"IronPython.dllが見つからない!!", MB_ICONERROR);
			}
			else {
				System::Diagnostics::Trace::WriteLine(args->Name);
			}

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
