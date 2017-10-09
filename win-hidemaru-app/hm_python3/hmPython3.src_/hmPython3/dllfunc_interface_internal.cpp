#include <windows.h>

#include "hidemaruexe_export.h"
#include "dllfunc_interface_internal.h"

using namespace std;

CDynamicValue TestDynamicVar;
void CDynamicValue::Clear() {
	this->num = 0;
	this->wstr = L"";
	this->type = TDynamicType::TypeUnknown;
}


// �G�ۂ̕ϐ��������񂩐��l���̔���p
MACRO_DLL intHM_t SetDynamicVar(const void* dynamic_value) {

	auto param_type = (CHidemaruExeExport::DLLFUNCPARAM)CHidemaruExeExport::Hidemaru_GetDllFuncCalledType(1);
	if (param_type == CHidemaruExeExport::DLLFUNCPARAM::WCHAR_PTR) {
		TestDynamicVar.wstr = wstring((wchar_t *)dynamic_value);
		TestDynamicVar.type = CDynamicValue::TDynamicType::TypeString;
		return 1;
	}
	else {
		TestDynamicVar.num = (intHM_t)dynamic_value;
		TestDynamicVar.type = CDynamicValue::TDynamicType::TypeInteger;
		return 1;
	}
}


intHM_t popnumvar = 0;
// �X�^�b�N�����ϐ����G�ۃ}�N��������o���B���������p
MACRO_DLL intHM_t PopNumVar() {
	return popnumvar;
}

// �ϐ����G�ۃ}�N��������o�����߂ɃX�^�b�N�B���������p
MACRO_DLL intHM_t PushNumVar(intHM_t i_tmp_num) {
	popnumvar = i_tmp_num;
	return 1;
}

// �X�^�b�N�����ϐ����G�ۃ}�N��������o���B���������p
static wstring popstrvar;
MACRO_DLL const wchar_t * PopStrVar() {
	return popstrvar.data();
}

// �ϐ����G�ۃ}�N��������o�����߂ɃX�^�b�N�B���������p
MACRO_DLL intHM_t PushStrVar(const wchar_t *sz_tmp_str) {
	popstrvar = sz_tmp_str;
	return 1;
}

