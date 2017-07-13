#include <windows.h>
#include <tchar.h>


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

// ここより上は全部マネージド

//----------ここだけネイティブ
#pragma unmanaged 
// ほかのマネージドコードより先に実行させるため。
// 先にここを定義して実行。アンマネージド
DllAssemblyResolver asmresolver;
#pragma managed
//----------ここだけネイティブ

// ここより後は全部マネージド

// リゾルバーの後に、フォームの定義行のコードが走るようにしなければならない。リゾルバーが何よりも先

#include "HmTextSynChrome.h"

//------------------------------------------------------------------------------------
#define MACRO_DLL extern "C" __declspec(dllexport)


// マクロから呼ばれる
MACRO_DLL intptr_t Show(HWND hWndHidemaru, wchar_t *pwszOption1) {

	HmTextSynChromeForm::form = gcnew HmTextSynChromeForm(hWndHidemaru, gcnew String(pwszOption1));

	// フォームの表示の必要性はない。// chromeブラウザの方で対応する
	// HmHtmlBaseForm::form->Show();

	return TRUE;
}

// 秀丸の該当プロセスを閉じたとき
MACRO_DLL intptr_t Dispose() {

	try {
		// まだ残っていたら(フォームを手動で閉じていたら、残っていない)
		if (HmTextSynChromeForm::form != nullptr) {

			//監視を終了
			HmTextSynChromeForm::form->Stop();

			// 閉じてNull
			HmTextSynChromeForm::form->Close();
			HmTextSynChromeForm::form = nullptr;

			// 秀丸が閉じる前までに可能な限りマネージドのインスタンスを解放して不正エラーが出にくい状態へと持っていく。
			GC::Collect();
			GC::WaitForPendingFinalizers();

		}
	}
	catch (Exception^) {}

	return TRUE;
}

// 秀丸の該当プロセスを閉じたとき
MACRO_DLL intptr_t DllDetachFunc_After_Hm866() {
	return Dispose();
}