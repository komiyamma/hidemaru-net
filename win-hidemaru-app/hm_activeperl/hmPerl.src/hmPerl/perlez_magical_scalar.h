#pragma once

#include "perlez_engine.h"

#define szMagicalVarDebugInfo		 "hm_debuginfo"
#define szMagicalVarMacroVarSimbol   "hm_Macro_Var_Simbol"
#define szMagicalVarMacroVarValue    "hm_Macro_Var_Value"
#define szMagicalVarMacroEval		 "hm_Macro_Eval"
#define szMagicalVarEditTotalText	 "hm_Edit_TotalText"
#define szMagicalVarEditSelectedText "hm_Edit_SelectedText"
#define szMagicalVarEditLineText     "hm_Edit_LineText"
#define szMagicalVarEditCursorPos    "hm_Edit_CursorPos"


//========================================================================
// PerlEzで特定のシンボルを一種の「プロパティ」のように見立てて、Get/Set用の関数をフックする。
//========================================================================
class CPerlEzMagicalScalar {
	// シンボルにアクセスする際にここに直前のものがかならずストックされる。
private:
	static wstring stocked_macro_var_simbol;

	//-------------------------------------------------------------------------
	// 返り値はstringで持っておく。PerlScopeへと値を返した時に、インスタンスの消滅を防ぐため。
	static string utf8_getvarofreturn;

	
	//-------------------------------------------------------------------------
	// プロパティに見立てたPerlScope上のグローバル変数に対して、「値を取得」の操作が行われた時、この関数が呼ばれる。
	// p_utf8_SelfName には、対象のグローバル変数のシンボル名そのものが渡ってくるので、これで処理を振り分け
	static LPCSTR GetHmComposedMagicScalarFunctions(LPVOID obj, LPCSTR p_utf8_SelfName);

	//-------------------------------------------------------------------------
	// 返り値はstringで持っておく。PerlScopeへと値を返した時に、インスタンスの消滅を防ぐため。
	static string utf8_setvarofreturn;

	//-------------------------------------------------------------------------
	// プロパティに見立てたPerlScope上のグローバル変数に対して、「値を設定」の操作が行われた時、この関数が呼ばれる。
	// p_utf8_SelfName には、対象のグローバル変数のシンボル名そのものが渡ってくるので、これで処理を振り分け
	static LPCSTR SetHmComposedMagicScalarFunctions(LPVOID obj, LPCSTR p_utf8_SelfName, LPCSTR p_utf8_Value);

public:
	//-------------------------------------------------------------------------
	// Get/Set用の関数をフックの共通関数の登録と、一種の「プロパティ」のように見立たてるシンボル名の登録
	//-------------------------------------------------------------------------
	static void BindMagicalScalarFunctions(CPerlEzEngine* module);


private:
	//-------------------------------------------------------------------------
	// フック用関数の各種イベントハンドラ
	//-------------------------------------------------------------------------
	struct Hm {

		static BOOL debuginfo(wstring utf16_value);

		struct Edit {
			struct Get {
				static string TotalText();
				static string SelectedText();
				static string LineText();
				static string CursorPos();
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


