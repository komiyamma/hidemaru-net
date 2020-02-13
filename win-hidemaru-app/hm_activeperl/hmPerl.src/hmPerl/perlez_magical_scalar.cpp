/*
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#include <windows.h>
#include <tchar.h>

#include "convert_string.h"
#include "perlez_magical_scalar.h"
#include "hidemaruexe_export.h"
#include "output_debugstream.h"
#include "self_dll_info.h"
#include "dllfunc_interface.h"
#include "dllfunc_interface_internal.h"

// �V���{���ɃA�N�Z�X����ۂɂ����ɒ��O�̂��̂����Ȃ炸�X�g�b�N�����B
wstring CPerlEzMagicalScalar::stocked_macro_var_simbol = L"";

// LPCSTR��Ԃ��̂Ŏ��̂��c�����߁A�K���Autf8_getvarofreturn.data()�����^�[������B
string CPerlEzMagicalScalar::utf8_getvarofreturn = "";

extern string utf8_lastEvalResult = "";

LPCSTR CPerlEzMagicalScalar::GetHmComposedMagicScalarFunctions(LPVOID obj, LPCSTR p_utf8_SelfName)
{
	utf8_getvarofreturn.clear();

	string utf8_SelfName = p_utf8_SelfName;

	if (utf8_SelfName == szMagicalVarVersion) {
		utf8_getvarofreturn = Hm::version();
	}
	else if (utf8_SelfName == szMagicalVarEditTotalText) {
		utf8_getvarofreturn = Hm::Edit::Get::TotalText();
	}
	else if (utf8_SelfName == szMagicalVarEditSelectedText) {
		utf8_getvarofreturn = Hm::Edit::Get::SelectedText();
	}
	else if (utf8_SelfName == szMagicalVarEditLineText) {
		utf8_getvarofreturn = Hm::Edit::Get::LineText();
	}
	else if (utf8_SelfName == szMagicalVarEditCursorPos) {
		utf8_getvarofreturn = Hm::Edit::Get::CursorPos();
	}
	else if (utf8_SelfName == szMagicalVarEditCursorPosFromMousePos) {
		utf8_getvarofreturn = Hm::Edit::Get::CursorPosFromMousePos();
	}
	// hm->Macro->Var(...)�ւƓǂݎ��̏ꍇ
	else if (utf8_SelfName == szMagicalVarMacroVarSimbol) {
		utf8_getvarofreturn = Hm::Macro::Get::VarSimbol();
	}

	// hm->Macro->Eval�̌���
	else if (utf8_SelfName == szMagicalVarMacroEvalResult) {
		string ret = utf8_lastEvalResult;
		utf8_lastEvalResult.clear();
		utf8_getvarofreturn = ret;
	}

	return utf8_getvarofreturn.data();
}

string CPerlEzMagicalScalar::Hm::version() {
	wstring utf16_Text = to_wstring(CHidemaruExeExport::hm_version);
	return utf16_to_utf8(utf16_Text);
}

string CPerlEzMagicalScalar::Hm::Edit::Get::TotalText() {
	wstring utf16_Text = CHidemaruExeExport::GetTotalText();
	return utf16_to_utf8(utf16_Text);
}

string CPerlEzMagicalScalar::Hm::Edit::Get::SelectedText() {
	wstring utf16_Text = CHidemaruExeExport::GetSelectedText();
	return utf16_to_utf8(utf16_Text);
}

string CPerlEzMagicalScalar::Hm::Edit::Get::LineText() {
	wstring utf16_Text = CHidemaruExeExport::GetLineText();
	return utf16_to_utf8(utf16_Text);
}

string CPerlEzMagicalScalar::Hm::Edit::Get::CursorPos() {
	auto pos = CHidemaruExeExport::GetCursorPos();
	return std::to_string(pos.lineno) + "," + std::to_string(pos.column);
}

string CPerlEzMagicalScalar::Hm::Edit::Get::CursorPosFromMousePos() {
	auto pos = CHidemaruExeExport::GetCursorPosFromMousePos();
	return std::to_string(pos.x) + "," + std::to_string(pos.y) + "," + std::to_string(pos.lineno) + "," + std::to_string(pos.column);
}


string CPerlEzMagicalScalar::Hm::Macro::Get::VarSimbol() {

	TestDynamicVar.Clear();
	auto dll_invocant = CSelfDllInfo::GetInvocantString();
	wstring cmd =
		L"##_tmp_dll_id_ret = dllfuncw( " + dll_invocant + L"\"SetDynamicVar\", " + stocked_macro_var_simbol + L");\n"
		L"##_tmp_dll_id_ret = 0;\n";
	BOOL success = CHidemaruExeExport::EvalMacro(cmd);

	// ���l�Ȃ�
	if (TestDynamicVar.type == CDynamicValue::TDynamicType::TypeInteger)
	{
		return std::to_string(TestDynamicVar.num).data();
	}
	// ������Ȃ�
	else {
		return utf16_to_utf8(TestDynamicVar.wstr).data();
	}
}




string CPerlEzMagicalScalar::utf8_setvarofreturn = "";
LPCSTR CPerlEzMagicalScalar::SetHmComposedMagicScalarFunctions(LPVOID obj, LPCSTR p_utf8_SelfName, LPCSTR p_utf8_Value)
{
	utf8_setvarofreturn.clear();

	string utf8_SelfName = p_utf8_SelfName;
	wstring utf16_value = utf8_to_utf16(p_utf8_Value);

	// hm->debuginfo(...)�����s
	if (utf8_SelfName == szMagicalVarDebugInfo) {
		CPerlEzMagicalScalar::Hm::debuginfo(utf16_value);
		return p_utf8_Value;
	}

	// hm->File->HmEncode(...)�ւƑ��
	else if (utf8_SelfName == szMagicalVarFileHmEncode) {
		int ret = CPerlEzMagicalScalar::Hm::File::Get::HmEncode(utf16_value);
		// �֐������ă|�C���^�����Ȃ��悤�ɁA�O���[�o���ɏ������
		utf8_setvarofreturn = to_string(ret);
		return utf8_setvarofreturn.data();
	}

	// hm->Macro->Eval(...)�����s
	else if (utf8_SelfName == szMagicalVarMacroEval) {
		// ���ʂ̃N���A
		utf8_lastEvalResult.clear();

		int ret = CPerlEzMagicalScalar::Hm::Macro::Eval(utf16_value);
		if (ret) {
			utf8_lastEvalResult = "";
		}
		else {
			utf8_lastEvalResult = "HidemaruMacroEvalException";
		}
		return ret ? p_utf8_Value : "";
	}

	// hm->Edit->TodalText(...)�ւƑ��
	else if (utf8_SelfName == szMagicalVarEditTotalText) {
		int ret = CPerlEzMagicalScalar::Hm::Edit::Set::TotalText(utf16_value);
		return ret ? p_utf8_Value : "";
	}

	// hm->Edit->SelectedText(...)�ւƑ��
	else if (utf8_SelfName == szMagicalVarEditSelectedText) {
		int ret = CPerlEzMagicalScalar::Hm::Edit::Set::SelectedText(utf16_value);
		return ret ? p_utf8_Value : "";
	}
	// hm->Edit->LineText(...)�ւƑ��
	else if (utf8_SelfName == szMagicalVarEditLineText) {
		int ret = CPerlEzMagicalScalar::Hm::Edit::Set::LineText(utf16_value);
		return ret ? p_utf8_Value : "";
	}

	// hm->Macro->Var(...)�ւƑ���̑O��
	else if (utf8_SelfName == szMagicalVarMacroVarSimbol) {
		int ret = CPerlEzMagicalScalar::Hm::Macro::Set::VarSimbol(utf16_value);
		return ret ? p_utf8_Value : "";
	}
	// hm->Macro->Var(...)�ւƑ���̌㔼
	else if (utf8_SelfName == szMagicalVarMacroVarValue) {
		int ret = CPerlEzMagicalScalar::Hm::Macro::Set::VarValue(utf16_value);
		return ret ? p_utf8_Value : "";
	}

	return p_utf8_Value;
}

int CPerlEzMagicalScalar::Hm::File::Get::HmEncode(wstring utf16_value) {
	int hm_encode = CHidemaruExeExport::AnalyzeEncoding(utf16_value);
	return hm_encode;
}

BOOL CPerlEzMagicalScalar::Hm::debuginfo(wstring utf16_value) {
	OutputDebugStream(utf16_value);
	return TRUE;
}

BOOL CPerlEzMagicalScalar::Hm::Macro::Eval(wstring utf16_value) {
	BOOL success = CHidemaruExeExport::EvalMacro(utf16_value);
	if (success) {
		return TRUE;
	}
	OutputDebugStream(L"�}�N���̎��s�Ɏ��s���܂����B\n");
	OutputDebugStream(L"�}�N�����e:\n");
	OutputDebugStream(utf16_value);
	return FALSE;
}


BOOL CPerlEzMagicalScalar::Hm::Edit::Set::TotalText(wstring utf16_value) {
	BOOL success = 0;

	auto dll_invocant = CSelfDllInfo::GetInvocantString();

	PushStrVar(utf16_value.data());
	wstring cmd =
		L"begingroupundo;\n"
		L"selectall;\n"
		L"insert dllfuncstrw( " + dll_invocant + L"\"PopStrVar\" );\n"
		L"endgroupundo;\n";
	success = CHidemaruExeExport::EvalMacro(cmd);

	return success;
}

BOOL CPerlEzMagicalScalar::Hm::Edit::Set::SelectedText(wstring utf16_value) {
	BOOL success = 0;

	auto dll_invocant = CSelfDllInfo::GetInvocantString();

	PushStrVar(utf16_value.data());
	wstring cmd = 
		L"if (selecting) {\n"
		L"insert dllfuncstrw( " + dll_invocant + L"\"PopStrVar\" );\n"
		L"};\n";
	success = CHidemaruExeExport::EvalMacro(cmd);

	return success;
}

BOOL CPerlEzMagicalScalar::Hm::Edit::Set::LineText(wstring utf16_value) {
	BOOL success = 0;

	auto dll_invocant = CSelfDllInfo::GetInvocantString();

	auto pos = CHidemaruExeExport::GetCursorPos();
	PushStrVar(utf16_value.data());
	wstring cmd =
		L"begingroupundo;\n"
		L"selectline;\n"
		L"insert dllfuncstrw( " + dll_invocant + L"\"PopStrVar\" );\n"
		L"moveto2 " + std::to_wstring(pos.column) + L", " + std::to_wstring(pos.lineno) + L";\n" +
		L"endgroupundo;\n";
	success = CHidemaruExeExport::EvalMacro(cmd);

	return success;
}

BOOL CPerlEzMagicalScalar::Hm::Macro::Set::VarSimbol(wstring utf16_value) {
	stocked_macro_var_simbol = utf16_value;
	return TRUE;
}

BOOL CPerlEzMagicalScalar::Hm::Macro::Set::VarValue(wstring utf16_value) {
	BOOL success = 0;

	auto dll_invocant = CSelfDllInfo::GetInvocantString();

	wchar_t start = stocked_macro_var_simbol[0];
	if (start == L'#') {
		// �����𐔒l�Ƀg���C�B�_���Ȃ�0����B
		intHM_t n = 0;
		try {
			n = (intHM_t)std::stoll(utf16_value);
		}
		catch (...) {}

		PushNumVar(n);
		wstring cmd = L" = dllfuncw( " + dll_invocant + L"\"PopNumVar\" );\n";
		cmd = stocked_macro_var_simbol + cmd;
		success = CHidemaruExeExport::EvalMacro(cmd);
	}
	else if (start == L'$') {

		PushStrVar(utf16_value.data());
		wstring cmd = L" = dllfuncstrw( " + dll_invocant + L"\"PopStrVar\" );\n";
		cmd = stocked_macro_var_simbol + cmd;
		success = CHidemaruExeExport::EvalMacro(cmd);
	}

	return success;
}

void CPerlEzMagicalScalar::BindMagicalScalarFunctions(CPerlEzEngine* module) {
	auto& engine = module->engine;

	// �S�Ẵv���p�e�B(�I�ȃO���[�o���ϐ�)�Ƀt�b�N�����AGet��Set�̊֐���o�^
	module->PerlEzSetMagicScalarFunctions(engine, CPerlEzMagicalScalar::GetHmComposedMagicScalarFunctions, CPerlEzMagicalScalar::SetHmComposedMagicScalarFunctions);

	// ��L�֐����@�\������O���[�o���ϐ���o�^���Ă����B
	auto list = {
		szMagicalVarDebugInfo,
		szMagicalVarVersion,
		szMagicalVarFileHmEncode,
		szMagicalVarEditTotalText,
		szMagicalVarEditSelectedText,
		szMagicalVarEditLineText,
		szMagicalVarEditCursorPos,
		szMagicalVarEditCursorPosFromMousePos,
		szMagicalVarMacroEval,
		szMagicalVarMacroEvalResult,
		szMagicalVarMacroVarSimbol,
		szMagicalVarMacroVarValue
	};

	for (auto var_name : list) {
		module->PerlEzSetMagicScalarName(engine, var_name);
	}
}