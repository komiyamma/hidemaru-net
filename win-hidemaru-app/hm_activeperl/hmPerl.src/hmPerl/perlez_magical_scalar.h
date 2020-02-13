/*
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#pragma once

#include "perlez_engine.h"

#define szMagicalVarDebugInfo		    "hm_debuginfo"
#define szMagicalVarVersion			    "hm_version"
#define szMagicalVarMacroVarSimbol      "hm_Macro_Var_Simbol"
#define szMagicalVarMacroVarValue       "hm_Macro_Var_Value"
#define szMagicalVarMacroEval		    "hm_Macro_Eval"
#define szMagicalVarMacroEvalResult	    "hm_Macro_Eval_Result"
#define szMagicalVarEditTotalText	    "hm_Edit_TotalText"
#define szMagicalVarEditSelectedText    "hm_Edit_SelectedText"
#define szMagicalVarEditLineText        "hm_Edit_LineText"
#define szMagicalVarFileHmEncode        "hm_File_HmEncode"
#define szMagicalVarFileHmEncodeResult  "hm_File_HmEncode_Result"
#define szMagicalVarEditCursorPos       "hm_Edit_CursorPos"
#define szMagicalVarEditCursorPosFromMousePos    "hm_Edit_CursorPosFromMousePos"


//========================================================================
// PerlEz�œ���̃V���{�������́u�v���p�e�B�v�̂悤�Ɍ����ĂāAGet/Set�p�̊֐����t�b�N����B
//========================================================================
class CPerlEzMagicalScalar {
	// �V���{���ɃA�N�Z�X����ۂɂ����ɒ��O�̂��̂����Ȃ炸�X�g�b�N�����B
private:
	static wstring stocked_macro_var_simbol;

	//-------------------------------------------------------------------------
	// �Ԃ�l��string�Ŏ����Ă����BPerlScope�ւƒl��Ԃ������ɁA�C���X�^���X�̏��ł�h�����߁B
	static string utf8_getvarofreturn;

	
	//-------------------------------------------------------------------------
	// �v���p�e�B�Ɍ����Ă�PerlScope��̃O���[�o���ϐ��ɑ΂��āA�u�l���擾�v�̑��삪�s��ꂽ���A���̊֐����Ă΂��B
	// p_utf8_SelfName �ɂ́A�Ώۂ̃O���[�o���ϐ��̃V���{�������̂��̂��n���Ă���̂ŁA����ŏ�����U�蕪��
	static LPCSTR GetHmComposedMagicScalarFunctions(LPVOID obj, LPCSTR p_utf8_SelfName);

	//-------------------------------------------------------------------------
	// �Ԃ�l��string�Ŏ����Ă����BPerlScope�ւƒl��Ԃ������ɁA�C���X�^���X�̏��ł�h�����߁B
	static string utf8_setvarofreturn;

	//-------------------------------------------------------------------------
	// �v���p�e�B�Ɍ����Ă�PerlScope��̃O���[�o���ϐ��ɑ΂��āA�u�l��ݒ�v�̑��삪�s��ꂽ���A���̊֐����Ă΂��B
	// p_utf8_SelfName �ɂ́A�Ώۂ̃O���[�o���ϐ��̃V���{�������̂��̂��n���Ă���̂ŁA����ŏ�����U�蕪��
	static LPCSTR SetHmComposedMagicScalarFunctions(LPVOID obj, LPCSTR p_utf8_SelfName, LPCSTR p_utf8_Value);

public:
	//-------------------------------------------------------------------------
	// Get/Set�p�̊֐����t�b�N�̋��ʊ֐��̓o�^�ƁA���́u�v���p�e�B�v�̂悤�Ɍ������Ă�V���{�����̓o�^
	//-------------------------------------------------------------------------
	static void BindMagicalScalarFunctions(CPerlEzEngine* module);


private:
	//-------------------------------------------------------------------------
	// �t�b�N�p�֐��̊e��C�x���g�n���h��
	//-------------------------------------------------------------------------
	struct Hm {

		static BOOL debuginfo(wstring utf16_value);
		static string version();

		struct File {
			struct Get {
				static int HmEncode();
			};
			struct Set {
				static int HmEncode(wstring utf16_value);
			};
		};

		struct Edit {
			struct Get {
				static string TotalText();
				static string SelectedText();
				static string LineText();
				static string CursorPos();
				static string CursorPosFromMousePos();
			};
			struct Set {
				static BOOL TotalText(wstring utf16_value);
				static BOOL SelectedText(wstring utf16_value);
				static BOOL LineText(wstring utf16_value);
			};

		};
		struct Macro {

			static BOOL Eval(wstring utf16_value);

			struct Get {
				static string VarSimbol();
			};
			struct Set {
				static BOOL VarSimbol(wstring utf16_value);
				static BOOL VarValue(wstring utf16_value);
			};
		};
	};
};


