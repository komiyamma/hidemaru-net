/*
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */


#include <windows.h>
#include <string>

#include "string_converter.h"
#include "hidemaruexe_export.h"
#include "hmLm.h"
#include "hmLmStatlcLib.h"


using namespace std;
using namespace System;


// ��̎蓮��BindDllHandle�������ōs���B�G��8.66�ȏ�
// �P�񂾂����s����Ηǂ��킯�ł͂Ȃ��Bdll���ǂݍ��܂�Ă���Ԃɂ�dll�l���ς���Ă��܂���������Ȃ����߁B(�����̎����ł�)
// ����āADoString��DoFile�̎����_�@�ɍX�V����B
static bool BindDllHandle() {
	// �G��8.66�ȏ�
	if (Hidemaru_GetDllFuncCalledType) {
		int dll = Hidemaru_GetDllFuncCalledType(-1); // ������dll�̌Ă΂�����`�F�b�N
		ILmStaticLib::BindDllHandle((IntPtr)dll);
		return true;
	}
	return false;
}


MACRO_DLL intHM_t SetCodePage(intHM_t cp) {
	ILmStaticLib::SetCodePage((IntPtr)cp);
	return TRUE;
}

// �G�ۂ̕ϐ��������񂩐��l���̔���p
MACRO_DLL intHM_t SetTmpVar(const void* dynamic_value) {
	int param_type = Hidemaru_GetDllFuncCalledType(1); // 1�Ԗڂ̈����̌^�B
	if (param_type == DLLFUNCPARAM_WCHAR_PTR) {
		return (intHM_t)ILmStaticLib::SetTmpVar(gcnew String((wchar_t *)dynamic_value));
	}
	else {
		return (intHM_t)ILmStaticLib::SetTmpVar((intHM_t)dynamic_value);
	}
}

MACRO_DLL intHM_t PopNumVar() {
	intHM_t num = (intHM_t)ILmStaticLib::PopTmpVar();
	return num;
}

static wstring strvarsopop;
MACRO_DLL const TCHAR * PopStrVar() {
	auto var = (String ^)ILmStaticLib::PopTmpVar();
	strvarsopop = String_to_tstring(var);
	return strvarsopop.data();
}

//------------------------------------------------------------------------------------
MACRO_DLL intHM_t GetNumVar(const TCHAR *sz_var_name) {
	BindDllHandle();

	return (intHM_t)ILmStaticLib::GetNumVar(gcnew String(sz_var_name));
}

MACRO_DLL intHM_t SetNumVar(const TCHAR *sz_var_name, intHM_t value) {
	BindDllHandle();

	return (intHM_t)ILmStaticLib::SetNumVar(gcnew String(sz_var_name), (IntPtr)value);
}

// �G�ۂ̃L���b�V���̂���
static wstring strvars;
MACRO_DLL const TCHAR * GetStrVar(const TCHAR *sz_var_name) {
	BindDllHandle();

	auto var = ILmStaticLib::GetStrVar(gcnew String(sz_var_name));
	strvars = String_to_tstring(var->ToString());
	return strvars.data();
}

MACRO_DLL intHM_t SetStrVar(const TCHAR *sz_var_name, const TCHAR *value) {
	BindDllHandle();

	return (intHM_t)ILmStaticLib::SetStrVar(gcnew String(sz_var_name), gcnew String(value));
}
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
MACRO_DLL intHM_t GetNumItemOfList(const TCHAR *sz_arr_name, const intHM_t index) {
	BindDllHandle();

	return (intHM_t)ILmStaticLib::GetNumItemOfList(gcnew String(sz_arr_name), (IntPtr)index);
}

MACRO_DLL intHM_t SetNumItemOfList(const TCHAR *sz_arr_name, const intHM_t index, const intHM_t value) {
	BindDllHandle();

	return (intHM_t)ILmStaticLib::SetNumItemOfList(gcnew String(sz_arr_name), (IntPtr)index, (IntPtr)value);
}

// �G�ۂ̃L���b�V���̂���
static wstring strvarsoflist;
MACRO_DLL const TCHAR * GetStrItemOfList(const TCHAR *sz_arr_name, const intHM_t index) {
	BindDllHandle();

	auto var = ILmStaticLib::GetStrItemOfList(gcnew String(sz_arr_name), (IntPtr)index);
	strvarsoflist = String_to_tstring(var->ToString());
	return strvarsoflist.data();
}

MACRO_DLL intHM_t SetStrItemOfList(const TCHAR *sz_arr_name, const intHM_t index, const TCHAR *value) {
	BindDllHandle();

	return (intHM_t)ILmStaticLib::SetStrItemOfList(gcnew String(sz_arr_name), (IntPtr)index, gcnew String(value));
}
//------------------------------------------------------------------------------------



//------------------------------------------------------------------------------------
MACRO_DLL intHM_t GetNumItemOfDict(const TCHAR *sz_arr_name, const TCHAR *key) {
	BindDllHandle();

	return (intHM_t)ILmStaticLib::GetNumItemOfDict(gcnew String(sz_arr_name), gcnew String(key));
}

MACRO_DLL intHM_t SetNumItemOfDict(const TCHAR *sz_arr_name, const TCHAR *key, const intHM_t value) {
	BindDllHandle();

	return (intHM_t)ILmStaticLib::SetNumItemOfDict(gcnew String(sz_arr_name), gcnew String(key), (IntPtr)value);
}

// �G�ۂ̃L���b�V���̂���

static wstring strvarsofdict;
MACRO_DLL const TCHAR * GetStrItemOfDict(const TCHAR *sz_arr_name, const TCHAR *key) {
	BindDllHandle();

	auto var = ILmStaticLib::GetStrItemOfDict(gcnew String(sz_arr_name), gcnew String(key));
	strvarsofdict = String_to_tstring(var->ToString());
	return strvarsofdict.data();
}

MACRO_DLL intHM_t SetStrItemOfDict(const TCHAR *sz_arr_name, const TCHAR *key, const TCHAR *value) {
	BindDllHandle();

	return (intHM_t)ILmStaticLib::SetStrItemOfDict(gcnew String(sz_arr_name), gcnew String(key), gcnew String(value));
}
//------------------------------------------------------------------------------------


MACRO_DLL intHM_t DoString(const TCHAR *szexpression) {
	BindDllHandle();

	// �����͂悭�ԈႦ��̂ł��������`�F�b�N�B���͏G��8.66�ȍ~�ł͂قƂ�Ǘ��p���Ȃ��̂Ŗ���
	if (Hidemaru_GetDllFuncCalledType) {
		auto rtn_type = Hidemaru_GetDllFuncCalledType(0); // 0�͕Ԃ�l�̌^
		if (rtn_type == DLLFUNCRETURN_CHAR_PTR || rtn_type == DLLFUNCRETURN_WCHAR_PTR) {
			MessageBox(NULL, L"�Ԃ�l�̌^���قȂ�܂��B\ndllfuncstr�ł͂Ȃ��Adllfuncw���𗘗p���Ă��������B", L"�Ԃ�l�̌^���قȂ�܂�", MB_ICONERROR);
		}

		auto arg_type = Hidemaru_GetDllFuncCalledType(1); // 1��1�Ԗڂ̈���
		if (arg_type != DLLFUNCPARAM_WCHAR_PTR) {
			MessageBox(NULL, L"�����̌^���قȂ�܂��B\ndllfunc�ł͂Ȃ��Adllfuncw���𗘗p���Ă��������B", L"�����̌^���قȂ�܂�", MB_ICONERROR);
		}
	}

	return (intHM_t)ILmStaticLib::DoString(gcnew String(szexpression));
}

MACRO_DLL intHM_t DoFile(const TCHAR *szfilename) {
	BindDllHandle();

	// �����͂悭�ԈႦ��̂ł��������`�F�b�N�B���͏G��8.66�ȍ~�ł͂قƂ�Ǘ��p���Ȃ��̂Ŗ���
	if (Hidemaru_GetDllFuncCalledType) {
		auto rtn_type = Hidemaru_GetDllFuncCalledType(0); // 0�͕Ԃ�l�̌^
		if (rtn_type == DLLFUNCRETURN_CHAR_PTR || rtn_type == DLLFUNCRETURN_WCHAR_PTR) {
			MessageBox(NULL, L"�Ԃ�l�̌^���قȂ�܂��B\ndllfuncstr�ł͂Ȃ��Adllfuncw���𗘗p���Ă��������B", L"�Ԃ�l�̌^���قȂ�܂�", MB_ICONERROR);
		}

		auto arg_type = Hidemaru_GetDllFuncCalledType(1); // 1��1�Ԗڂ̈���
		if (arg_type != DLLFUNCPARAM_WCHAR_PTR) {
			MessageBox(NULL, L"�����̌^���قȂ�܂��B\ndllfunc�ł͂Ȃ��Adllfuncw���𗘗p���Ă��������B", L"�����̌^���قȂ�܂�", MB_ICONERROR);
		}
	}

	return (intHM_t)ILmStaticLib::DoFile(gcnew String(szfilename));
}

MACRO_DLL intHM_t DestroyScope() {
	return (intHM_t)ILmStaticLib::DestroyScope();
}

MACRO_DLL intHM_t DllDetachFunc_After_Hm866() {
	return DestroyScope();
}