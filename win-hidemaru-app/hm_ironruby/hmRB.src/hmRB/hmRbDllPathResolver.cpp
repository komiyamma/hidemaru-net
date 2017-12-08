/* 
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */


using namespace System;
using namespace System::Reflection;

// IronRuby.Libraries.dllとhmRb.dllを、hidemaru.exeとは異なる場所に置いた際の対処
static Assembly^ CurrentDomain_AssemblyResolve(Object^ sender, ResolveEventArgs^ args) {
	try
	{
		// IronRuby.Libraries.dllが見つからない場合に、hmRb.dllと同じディレクトリにIronRuby.Libraries.dllがあるなら、それを読みだす。
		// このようなことが起きるのは、hidemaru.exeとは異なる場所にhmRb.dllとIronRuby.Libraries.dllを置いた場合
		auto requestingAssembly = args->RequestingAssembly;
		auto requestedAssembly = gcnew AssemblyName(args->Name);

		if (requestedAssembly->Name == "IronRuby.Libraries" ) {

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

// ここより上は全部マネージド

//----------ここだけネイティブ
#pragma unmanaged 
// ほかのマネージドコードより先に実行させるため。
// 先にここを定義して実行。アンマネージド
DllAssemblyResolver asmresolver;
#pragma managed
//----------ここだけネイティブ

