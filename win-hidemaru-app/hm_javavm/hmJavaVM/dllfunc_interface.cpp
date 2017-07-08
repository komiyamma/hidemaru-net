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




MACRO_DLL intHM_t CallMethod(const TCHAR *class_name, TCHAR *method_name) {
	if (CreateScope() == 0)
	{
		return 0;
	}

	// DoStringされる度にdllのBindの在り方を確認更新する。
	CSelfDllInfo::SetBindDllHandle();

	auto rtn_type = (CHidemaruExeExport::DLLFUNCRETURN)CHidemaruExeExport::Hidemaru_GetDllFuncCalledType(0); // 0は返り値の型
	if (rtn_type == CHidemaruExeExport::DLLFUNCRETURN::CHAR_PTR || rtn_type == CHidemaruExeExport::DLLFUNCRETURN::WCHAR_PTR) {
		MessageBox(NULL, L"返り値の型が異なります。\ndllfuncstrではなく、dllfuncw文を利用してください。", L"返り値の型が異なります", MB_ICONERROR);
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

	// このプロセスで最初の１回
	if (!CJavaVMEngine::HmCalled) {
		bool success = CJavaVMEngine::CallStaticEntryMethod(L"hidemaru/Hm", L"_Init");
		if (!success) {
			return false;
		}
		CJavaVMEngine::HmCalled = true;
	}

	// JavaVMがクラスを探し出すパスをダイナミックに追加する
	CJavaVMEngine::CallStaticEntryMethod(L"hidemaru/Hm", L"_AddClassPath");

	// クラスは、「.」区切りで記載しているなら、「/」区切りに直す
	wstring wstr_class_name = class_name;
	std::replace(wstr_class_name.begin(), wstr_class_name.end(), '.', '/');

	bool success = CJavaVMEngine::CallStaticEntryMethod(wstr_class_name.c_str(), method_name);
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