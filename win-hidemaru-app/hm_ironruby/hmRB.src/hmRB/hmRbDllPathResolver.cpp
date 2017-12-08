/* 
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */


using namespace System;
using namespace System::Reflection;

// IronRuby.Libraries.dll��hmRb.dll���Ahidemaru.exe�Ƃ͈قȂ�ꏊ�ɒu�����ۂ̑Ώ�
static Assembly^ CurrentDomain_AssemblyResolve(Object^ sender, ResolveEventArgs^ args) {
	try
	{
		// IronRuby.Libraries.dll��������Ȃ��ꍇ�ɁAhmRb.dll�Ɠ����f�B���N�g����IronRuby.Libraries.dll������Ȃ�A�����ǂ݂����B
		// ���̂悤�Ȃ��Ƃ��N����̂́Ahidemaru.exe�Ƃ͈قȂ�ꏊ��hmRb.dll��IronRuby.Libraries.dll��u�����ꍇ
		auto requestingAssembly = args->RequestingAssembly;
		auto requestedAssembly = gcnew AssemblyName(args->Name);

		if (requestedAssembly->Name == "IronRuby.Libraries" ) {

			// ����dll���̂�u���Ă���t�H���_�ɓǂݍ��ݑΏۂ̃A�Z���u�������邩������Ȃ��B
			String^ self_full_path = System::Reflection::Assembly::GetExecutingAssembly()->Location;
			String^ self_dir = System::IO::Path::GetDirectoryName(self_full_path);

			auto targetfullpath = self_dir + R"(\)" + requestedAssembly->Name + ".dll";

			if (System::IO::File::Exists(targetfullpath))
			{
				return Assembly::LoadFile(targetfullpath);
			}
			// ���̂悤�ȃt���p�X���w�肳��Ă���ꍇ(�t���p�X���w�肵��������)
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

// ��������͑S���}�l�[�W�h

//----------���������l�C�e�B�u
#pragma unmanaged 
// �ق��̃}�l�[�W�h�R�[�h����Ɏ��s�����邽�߁B
// ��ɂ������`���Ď��s�B�A���}�l�[�W�h
DllAssemblyResolver asmresolver;
#pragma managed
//----------���������l�C�e�B�u

