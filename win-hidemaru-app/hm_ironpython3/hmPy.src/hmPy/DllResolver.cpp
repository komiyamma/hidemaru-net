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
			// IronPython.dll�����߂��Ă���A�Ȃ����APublic Key Token�������Ɛݒ肳��Ă���B
			auto targetName = requestedAssembly->Name->ToUpper();

			if (targetName == "IRONPYTHON" || targetName == "IRONPYTHON.MODULES" || targetName == "MICROSOFT.DYNAMIC" || targetName == "MICROSOFT.SCRIPTING" || targetName == "MICROSOFT.SCRIPTING.METADATA") {
				System::Diagnostics::Trace::WriteLine(args->Name);


				wchar_t message[256] = L"";
				
				wsprintf
					(
					message,
					L"http://IronPython.net/" L"����A\n"
					L"�Ώۂ�IronPython ���C���X�g�[�����Ă��������B\n"
					L"�u.msi�C���X�g�[���[�v�ł𗘗p���Ă��������B\n"
					L"%s",
					IRONPYTHON_VERSION
					);

				MessageBox(NULL, message, L"IronPython.dll��������Ȃ�!!", MB_ICONERROR);
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



//----------���������l�C�e�B�u
#pragma unmanaged 
// �ق��̃}�l�[�W�h�R�[�h����Ɏ��s�����邽�߁B
// ��ɂ������`���Ď��s�B�A���}�l�[�W�h
DllAssemblyResolver asmresolver;
#pragma managed
//----------���������l�C�e�B�u

//------------------------------------------------------------------------------------------------
