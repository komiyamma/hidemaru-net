#include <stdio.h>
#include <windows.h>

#include "convert_string.h"
#include "hidemaruexe_export.h"
#include "self_dll_info.h"

#include "dllfunc_interface.h"
#include "dllfunc_interface_internal.h"

#include "python_engine.h"

using namespace std;



static int CreateScope() {

	// 既にエンジンが構築されているなら
	if (PythonEngine::IsValid()) {
		return TRUE;
	}

	// エンジン構築。結果をそのまま返す
	BOOL isValid = PythonEngine::Create();
	return isValid;
}



//------------------------------------------------------------------------------------
MACRO_DLL intHM_t GetNumVar(const wchar_t *utf16_simbol) {
	if (CreateScope() == 0)
	{
		return 0;
	}

	CSelfDllInfo::SetBindDllHandle();

	intHM_t r = PythonEngine::GetNumVar(utf16_simbol);

	return r;
}

MACRO_DLL intHM_t SetNumVar(const wchar_t *utf16_simbol, intHM_t value) {
	if (CreateScope() == 0)
	{
		return 0;
	}

	CSelfDllInfo::SetBindDllHandle();

	BOOL r = PythonEngine::SetNumVar(utf16_simbol, value);

	return (intHM_t)r;
}

// 秀丸のキャッシュのため
static wstring strvars;
MACRO_DLL const wchar_t * GetStrVar(const wchar_t *utf16_simbol) {
	// クリア必須
	strvars.clear();

	if (CreateScope() == 0)
	{
		return 0;
	}

	CSelfDllInfo::SetBindDllHandle();

	strvars = PythonEngine::GetStrVar(utf16_simbol);

	return strvars.c_str();
}

MACRO_DLL intHM_t SetStrVar(const wchar_t *utf16_simbol, const wchar_t *utf16_value) {
	if (CreateScope() == 0)
	{
		return 0;
	}

	CSelfDllInfo::SetBindDllHandle();

	BOOL r = PythonEngine::SetStrVar(utf16_simbol, utf16_value);

	return (intHM_t)r;
}

MACRO_DLL intHM_t DoString(const wchar_t *utf16_expression) {
	if (CreateScope() == 0)
	{
		return 0;
	}

	// DoStringされる度にdllのBindの在り方を確認更新する。
	CSelfDllInfo::SetBindDllHandle();

	//-------------------------------------------------------------------------
	// ほとんどの場合、この「DoString」しか使われないハズなので、
	// この関数だけチェックしておく。
	//-------------------------------------------------------------------------
	auto rtn_type = (CHidemaruExeExport::DLLFUNCRETURN)CHidemaruExeExport::Hidemaru_GetDllFuncCalledType(0); // 0は返り値の型
	if (rtn_type == CHidemaruExeExport::DLLFUNCRETURN::CHAR_PTR || rtn_type == CHidemaruExeExport::DLLFUNCRETURN::WCHAR_PTR) {
		MessageBox(NULL, L"返り値の型が異なります。\ndllfuncstrではなく、dllfuncw文を利用してください。", L"返り値の型が異なります", MB_ICONERROR);
	}

	auto arg_type = (CHidemaruExeExport::DLLFUNCPARAM)CHidemaruExeExport::Hidemaru_GetDllFuncCalledType(1); // 1は1番目の引数
	if (arg_type != CHidemaruExeExport::DLLFUNCPARAM::WCHAR_PTR) {
		MessageBox(NULL, L"引数の型が異なります。\ndllfuncではなく、dllfuncw文を利用してください。", L"引数の型が異なります", MB_ICONERROR);
	}

	PythonEngine::DoString(utf16_expression);

	// 失敗したら0
	return 0;
}


MACRO_DLL intHM_t DestroyScope() {
	if (CreateScope() == 0)
	{
		return 0;
	}

	// DoStringされる度にdllのBindの在り方を確認更新する。
	CSelfDllInfo::SetBindDllHandle();

	PythonEngine::Destroy();

	return TRUE;
}


MACRO_DLL intHM_t DllDetachFunc_After_Hm866() {
	return DestroyScope();
}