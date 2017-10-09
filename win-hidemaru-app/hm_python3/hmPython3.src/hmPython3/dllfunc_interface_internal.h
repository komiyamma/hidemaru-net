#pragma once

#include <string>
#include "dllfunc_interface.h"

using namespace std;

//========================================================================
/// ���̃t�@�C�����猩�Ă��邽�߂�extern�B���ۂ�.cpp�̕����݂Ă�
//========================================================================
MACRO_DLL const wchar_t * PopStrVar();
MACRO_DLL intHM_t PushStrVar(const wchar_t *sz_tmp_str);
MACRO_DLL intHM_t PopNumVar();
MACRO_DLL intHM_t PushNumVar(intHM_t i_tmp_num);


//========================================================================
// �u���l�������񂩕s���ȏG�ۃ}�N���ϐ��v���u�G�ۃ}�N������dll�ցv�Ɠn�����A
// Hidemaru_GetDllFuncCalledType ���g�����ƂŁA�U�蕪����B�V���O���g���ϐ�
struct CDynamicValue {
	// ���l�������񂩂ǂ��炩
	intHM_t num;
	wstring wstr;

	enum TDynamicType { TypeUnknown = 0, TypeInteger = 1, TypeString = 2 };
	TDynamicType type;

	void Clear();
};

extern CDynamicValue TestDynamicVar;


