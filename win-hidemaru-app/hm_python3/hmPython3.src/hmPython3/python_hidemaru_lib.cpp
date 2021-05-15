/* 
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#include "python_hidemaru_lib.h"
#include "pybind11.h"

#include "hm_original_encode_mapfunc.h"

#include "resource.h"

using namespace pybind11::literals;



namespace Hidemaru {
	void SetMacroVarAndMakeMacroKeyArray(const pybind11::tuple &value_args, const pybind11::tuple &type_args, std::vector<std::wstring> &varname_list);

	void ClearMacroVarAndUpdateArgs(std::vector<std::wstring> &varname_list, pybind11::list &args, const pybind11::tuple &value_args);

	// バージョンの取得
	double GetVersion() {
		return CHidemaruExeExport::hm_version;
	}

	// オブジェクトとして捉え、文字列に変換して出力
	void DebugInfo(const py::object message) {
		auto str = py::str(message);
		OutputDebugStream(utf8_to_utf16(str));
	}

	std::string Edit_GetFilePath() {

		const int WM_HIDEMARUINFO = WM_USER + 181;
		const int HIDEMARUINFO_GETFILEFULLPATH = 4;
		HWND hWndHidemaru = CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle();
		if (hWndHidemaru) {
			wchar_t filepath[MAX_PATH * 3] = L"";
			LRESULT cwch = SendMessageW(hWndHidemaru, WM_HIDEMARUINFO, HIDEMARUINFO_GETFILEFULLPATH, (LPARAM)filepath);
			return utf16_to_utf8(filepath);
		}

		return "";
	}

	// 秀丸で現在編集しているテキスト全体の取得
	std::string Edit_GetTotalText() {
		wstring utf16_value = CHidemaruExeExport::GetTotalText();
		return utf16_to_utf8(utf16_value);
	}

	// 秀丸で現在編集しているテキスト全体の設定
	BOOL Edit_SetTotalText(const std::string utf8_value) {
		BOOL success = 0;

		auto dll_invocant = CSelfDllInfo::GetInvocantString();

		wstring utf16_value = utf8_to_utf16(utf8_value);
		PushStrVar(utf16_value.data());
		wstring cmd =
			L"begingroupundo;\n"
			L"selectall;\n"
			L"insert dllfuncstrw( " + dll_invocant + L"\"PopStrVar\" );\n"
			L"endgroupundo;\n";
		success = CHidemaruExeExport::EvalMacro(cmd);

		return success;
	}

	// 秀丸で現在編集している選択テキストの取得
	std::string Edit_GetSelectedText() {
		wstring utf16_value = CHidemaruExeExport::GetSelectedText();
		return utf16_to_utf8(utf16_value);
	}

	// 秀丸で現在編集している選択テキストの設定
	BOOL Edit_SetSelectedText(const std::string utf8_value) {
		BOOL success = 0;

		auto dll_invocant = CSelfDllInfo::GetInvocantString();

		wstring utf16_value = utf8_to_utf16(utf8_value);
		PushStrVar(utf16_value.data());
		wstring cmd =
			L"if (selecting) {\n"
			L"insert dllfuncstrw( " + dll_invocant + L"\"PopStrVar\" );\n"
			L"};\n";
		success = CHidemaruExeExport::EvalMacro(cmd);

		return success;
	}

	// 秀丸で現在編集しているテキストの中で、カーソルがある行の内容を取得
	std::string Edit_GetLineText() {
		wstring utf16_value = CHidemaruExeExport::GetLineText();
		return utf16_to_utf8(utf16_value);
	}

	// 秀丸で現在編集しているテキストの中で、カーソルがある行の内容を設定
	BOOL Edit_SetLineText(const std::string utf8_value) {
		BOOL success = 0;

		auto dll_invocant = CSelfDllInfo::GetInvocantString();

		auto pos = CHidemaruExeExport::GetCursorPos();

		wstring utf16_value = utf8_to_utf16(utf8_value);
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

	//-------------------------------------------------------------------------
	// 指定対象のファイルの秀丸encodeを得る。秀丸自体の「動作環境」-「ファイル」ー「エンコード1」の設定に従うので注意。
	// 汎用のエンコード取得機能ではなく、使用者が使っている秀丸の設定の元、その該当の秀丸でファイルを開いたとしたら、
	// 何のエンコードで開かれるのかを取得する機能となる。
	int File_GetHmEncode(const std::string utf8_filename) {
		wstring utf16_filename = utf8_to_utf16(utf8_filename);
		int ret = CHidemaruExeExport::AnalyzeEncoding(utf16_filename);
		return ret;
	}

	struct TEncodingParam {
		int ms_codepage;
		int hm_encode;
		string py_encoding;
	};

	TEncodingParam _GetEncodingParamFromHmEncode(const int hm_encode) {
		/*
		0,      // Unknown
		932,    // encode = 1 ANSI/OEM Japanese; Japanese (Shift-JIS)
		1200,   // encode = 2 Unicode UTF-16, little-endian
		51932,  // encode = 3 EUC
		50221,  // encode = 4 JIS
		65000,  // encode = 5 UTF-7
		65001,  // encode = 6 UTF-8
		1201,   // encode = 7 Unicode UTF-16, big-endian
		1252,   // encode = 8 欧文 ANSI Latin 1; Western European (Windows)
		936,    // encode = 9 簡体字中国語 ANSI/OEM Simplified Chinese (PRC, Singapore); Chinese Simplified (GB2312)
		950,    // encode =10 繁体字中国語 ANSI/OEM Traditional Chinese (Taiwan; Hong Kong SAR, PRC); Chinese Traditional (Big5)
		949,    // encode =11 韓国語 ANSI/OEM Korean (Unified Hangul Code)
		1361,   // encode =12 韓国語 Korean (Johab)
		1250,   // encode =13 中央ヨーロッパ言語 ANSI Central European; Central European (Windows)
		1257,   // encode =14 バルト語 ANSI Baltic; Baltic (Windows)
		1253,   // encode =15 ギリシャ語 ANSI Greek; Greek (Windows)
		1251,   // encode =16 キリル言語 ANSI Cyrillic; Cyrillic (Windows)
		42,     // encode =17 シンボル
		1254,   // encode =18 トルコ語 ANSI Turkish; Turkish (Windows)
		1255,   // encode =19 ヘブライ語 ANSI Hebrew; Hebrew (Windows)
		1256,   // encode =20 アラビア語 ANSI Arabic; Arabic (Windows)
		874,    // encode =21 タイ語 ANSI/OEM Thai (same as 28605, ISO 8859-15); Thai (Windows)
		1258,   // encode =22 ベトナム語 ANSI/OEM Vietnamese; Vietnamese (Windows)
		10001,  // encode =23 x-mac-japanese Japanese (Mac)
		850,    // encode =24 OEM/DOS
		0,      // encode =25 その他
		12000,  // encode =26 Unicode (UTF-32) little-endian
		12001,  // encode =27 Unicode (UTF-32) big-endian
		*/
		TEncodingParam param;

		if (hm_encode <= 0) {
			param.hm_encode = hm_encode;
			param.ms_codepage = 0;
			param.py_encoding = "";

			return param;
		}
			
		static const int code_arr[] = {
			0,
			932,
			1200,
			51932,
			50221,
			65000,
			65001,
			1201,
			1252,
			936,
			950,
			949,
			1361,
			1250,
			1257,
			1253,
			1251,
			42,
			1254,
			1255,
			1256,
			874,
			1258,
			10001,
			850,
			0,
			12000,
			12001
		};

		// コードページの一覧表の個数
		const int code_arr_length = sizeof(code_arr) / sizeof(int);

		// 範囲チェック(秀丸が返したencode値が個数をはみ出していたら...)
		if (hm_encode >= code_arr_length) {
			param.hm_encode = hm_encode;
			param.ms_codepage = 0;
			param.py_encoding = "";

			return param;
		}
		else {
			param.hm_encode = hm_encode;
			param.ms_codepage = code_arr[hm_encode];
		}



		switch (param.ms_codepage) {
		case 0: {
			param.py_encoding = "";
			break;
		}
		case 932: {
			param.py_encoding = "cp932";
			break;
		}
		case 1200: {
			param.py_encoding = "utf_16_le";
			break;
		}
		case 51932: {
			param.py_encoding = "euc_jp";
			break;
		}
		case 50221: {
			param.py_encoding = "iso2022_jp";
			break;
		}
		case 65000: {
			param.py_encoding = "utf_7";
			break;
		}
		case 65001: {
			param.py_encoding = "utf_8";
			break;
		}
		case 1201: {
			param.py_encoding = "utf_16_be";
			break;
		}
		case 1252: {
			param.py_encoding = "cp1252";
			break;
		}
		case 936: {
			param.py_encoding = "gb2312";
			break;
		}
		case 950: {
			param.py_encoding = "big5";
			break;
		}
		case 949: {
			param.py_encoding = "cp949";
			break;
		}
		case 1361: {
			param.py_encoding = "cp1361";
			break;
		}
		case 1250: {
			param.py_encoding = "cp1250";
			break;
		}
		case 1257: {
			param.py_encoding = "cp1257";
			break;
		}
		case 1253: {
			param.py_encoding = "cp1253";
			break;
		}
		case 1251: {
			param.py_encoding = "cp1251";
			break;
		}
		case 42: {
			param.py_encoding = "symbol";
			break;
		}
		case 1254: {
			param.py_encoding = "cp1254";
			break;
		}
		case 1255: {
			param.py_encoding = "cp1255";
			break;
		}
		case 1256: {
			param.py_encoding = "cp1256";
			break;
		}
		case 874: {
			param.py_encoding = "iso8859_15";
			break;
		}
		case 1258: {
			param.py_encoding = "cp1258";
			break;
		}
		case 10001: {
			param.py_encoding = "x_mac_japanese";
			break;
		}
		case 850: {
			param.py_encoding = "cp850";
			break;
		}
		case 12000: {
			param.py_encoding = "utf_32_le";
			break;
		}
		case 12001: {
			param.py_encoding = "utf_32_be";
			break;
		}
		}

		return param;
	}

	py::tuple File_GetEncodingFromHmEncode(const int hm_encode) {
		auto param = _GetEncodingParamFromHmEncode(hm_encode);

		return py::make_tuple(param.py_encoding, param.ms_codepage, param.hm_encode);
	}

	py::tuple File_GetEncodingFromFile(const std::string utf8_filename) {
		int hm_encode = File_GetHmEncode(utf8_filename);

		auto param = _GetEncodingParamFromHmEncode(hm_encode);

		return py::make_tuple(param.py_encoding, param.ms_codepage, param.hm_encode);
	}

	// 指定の文字コードを開く
	py::tuple  File_ReadAllText(const std::string utf8_filename, int hm_encode) {
		wstring utf16_filename = utf8_to_utf16(utf8_filename);
		UINT encoded_character_count = 0;
		bool success = 0;
		wstring utf16_value = CHidemaruExeExport::LoadFileUnicode(utf16_filename, hm_encode, &encoded_character_count, NULL, NULL, &success);
		return py::make_tuple(success, utf16_to_utf8(utf16_value));
	}

	// カーソルの位置。扱いやすいのでpythonのタプル(lineno, column)で返しておく
	py::tuple Edit_GetCursorPos() {
		auto ret = CHidemaruExeExport::GetCursorPos();
		return py::make_tuple(ret.lineno, ret.column);
	}

	// マウスの座標の下に相当する(lineno, column)の値。扱いやすいのでpythonのタプル(lineno, column, x, y)で返しておく
	// x, yは本当にwin32apiでのマウスの座標
	py::tuple Edit_GetCursorPosFromMousePos() {
		auto ret = CHidemaruExeExport::GetCursorPosFromMousePos();
		return py::make_tuple(ret.lineno, ret.column, ret.x, ret.y);
	}

	BOOL Macro_IsExecuting() {

		if (CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle) {
			HWND hHidemaruWindow = CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle();
			const int WM_ISMACROEXECUTING = WM_USER + 167;
			LRESULT r = SendMessageW(hHidemaruWindow, WM_ISMACROEXECUTING, 0, 0);
			return (BOOL)r;
		}

		return FALSE;
	}

	// シンボルの値の取得
	py::object Macro_GetVar(const std::string utf8_simbol) {

		wstring utf16_simbol = utf8_to_utf16(utf8_simbol);

		TestDynamicVar.Clear();
		auto dll_invocant = CSelfDllInfo::GetInvocantString();
		wstring cmd =
			L"##_tmp_dll_id_ret = dllfuncw( " + dll_invocant + L"\"SetDynamicVar\", " + utf16_simbol + L");\n"
			L"##_tmp_dll_id_ret = 0;\n";
		BOOL success = CHidemaruExeExport::EvalMacro(cmd);

		// 数値なら
		if (TestDynamicVar.type == CDynamicValue::TDynamicType::TypeInteger)
		{
			py::object r = py::cast(TestDynamicVar.num);
			return r;
		}
		// 文字列なら
		else {
			string utf8_value = utf16_to_utf8(TestDynamicVar.wstr);
			py::object r = py::cast(utf8_value);
			return r;
		}
	}

	// シンボルの値の設定
	BOOL Macro_SetVar(const std::string utf8_simbol, const py::object value) {
		BOOL success = 0;

		auto dll_invocant = CSelfDllInfo::GetInvocantString();

		wstring utf16_simbol = utf8_to_utf16(utf8_simbol);
		wchar_t start = utf16_simbol[0];
		if (start == L'#') {

			string utf8_value = py::str(value);
			wstring utf16_value = utf8_to_utf16(utf8_value);

			// 数字を数値にトライ。ダメなら0だよ。
			intHM_t n = 0;
			try {
				n = (intHM_t)std::stoll(utf16_value);
			}
			catch (...) {}

			PushNumVar(n);
			wstring cmd = L" = dllfuncw( " + dll_invocant + L"\"PopNumVar\" );\n";
			cmd = utf16_simbol + cmd;
			success = CHidemaruExeExport::EvalMacro(cmd);
		}
		else if (start == L'$') {

			string utf8_value = py::str(value);
			wstring utf16_value = utf8_to_utf16(utf8_value);

			PushStrVar(utf16_value.data());
			wstring cmd = L" = dllfuncstrw( " + dll_invocant + L"\"PopStrVar\" );\n";
			cmd = utf16_simbol + cmd;
			success = CHidemaruExeExport::EvalMacro(cmd);
		}

		return success;
	}

	// pythonの中から秀丸マクロ文字列を実行
	py::tuple Macro_Eval(const std::string utf8_expression) {
		wstring utf16_expression = utf8_to_utf16(utf8_expression);

		BOOL success = CHidemaruExeExport::EvalMacro(utf16_expression);
		if (success) {
			return py::make_tuple(success, "", "");
		}
		else {
			OutputDebugStream(L"マクロの実行に失敗しました。\n");
			OutputDebugStream(L"マクロ内容:\n");
			OutputDebugStream(utf16_expression);
			return py::make_tuple( success, "", "HidemaruMacroEvalException");
		}
	}

	py::tuple Macro_Exec_EvalMemory(string utf8_expression) {
		if (CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle) {
			wstring utf16_expression = utf8_to_utf16(utf8_expression);

			HWND hHidemaruWindow = CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle();
			const int WM_REMOTE_EXECMACRO_MEMORY = WM_USER + 272;

			WCHAR wszReturn[65535];
			*(WORD*)wszReturn = sizeof(wszReturn) / sizeof(wszReturn[0]); // 最初のバイトにバッファーのサイズを格納することで秀丸本体がバッファーサイズの上限を知る。
			LRESULT lRet = SendMessage(hHidemaruWindow, WM_REMOTE_EXECMACRO_MEMORY, (WPARAM)wszReturn, (LPARAM)utf16_expression.c_str());
			if (lRet) {
				string utf8_message = utf16_to_utf8(wszReturn);
				py::str py_message = py::str(utf8_message);
				return py::make_tuple(lRet, py_message, "");
			}
			else {
				OutputDebugStream(L"マクロの実行に失敗しました。\n");
				OutputDebugStream(L"マクロ内容:\n");
				OutputDebugStream(utf16_expression);
				return py::make_tuple(lRet, "", "HidemaruMacroExecEvalException");
			}
		}
		return py::make_tuple(0, "", "HidemaruMacroExecEvalException");
	}


	
	/** joins a vector of strings into a single string */
	std::wstring StringJoin(const std::vector<std::wstring> &strs, const std::wstring delim)
	{
		if (strs.size() == 0) return L"";
		std::vector<wchar_t> res;
		for (size_t i = 0; i < strs.size() - 1; ++i)
		{
			for (auto c : strs[i]) res.push_back(c);
			for (auto c : delim) res.push_back(c);
		}
		for (auto c : strs[strs.size() - 1]) res.push_back(c);
		return std::wstring{ res.begin(), res.end() };
	}

	// pythonの中から秀丸関数ステートメントを実行
	py::tuple Macro_Function(const std::string utf8_funcname, const py::tuple value_args, const py::tuple type_args) {
		wstring utf16_funcname = utf8_to_utf16(utf8_funcname);

		string num = to_string(value_args.size());

		vector<wstring> varname_list;
		SetMacroVarAndMakeMacroKeyArray(value_args, type_args, varname_list);

		wstring arg_varname_list = StringJoin(varname_list, L", ");
		wstring utf16_expression = utf16_funcname + L"(" + arg_varname_list + L")";

		// GetVar相当-------------------------
		TestDynamicVar.Clear();
		auto dll_invocant = CSelfDllInfo::GetInvocantString();
		wstring cmd =
			L"##_tmp_dll_id_ret = dllfuncw( " + dll_invocant + L"\"SetDynamicVar\", " + utf16_expression + L");\n"
			L"##_tmp_dll_id_ret = 0;\n";
		BOOL success = CHidemaruExeExport::EvalMacro(cmd);
		py::object r = py::cast<py::none>(Py_None);
		// 数値なら
		if (TestDynamicVar.type == CDynamicValue::TDynamicType::TypeInteger)
		{
			r = py::cast(TestDynamicVar.num);
		}
		// 文字列なら
		else if (TestDynamicVar.type == CDynamicValue::TDynamicType::TypeUnknown) {
			; // スルー
		} else {
			string utf8_value = utf16_to_utf8(TestDynamicVar.wstr);
			r = py::cast(utf8_value);
		}

		
		py::list args;
		ClearMacroVarAndUpdateArgs(varname_list, args, value_args);

		if (success) {
			return py::make_tuple(r, "", "", args);
		}
		else {
			OutputDebugStream(L"マクロの実行に失敗しました。\n");
			OutputDebugStream(L"マクロ内容:\n");
			OutputDebugStream(utf16_expression);
			return py::make_tuple(r, "", "HidemaruMacroEvalException", args);
		}
	}


	// pythonの中から秀丸マクロステートメントを実行
	py::tuple Macro_Statement(const std::string utf8_funcname, const py::tuple value_args, const py::tuple type_args) {
		wstring utf16_funcname = utf8_to_utf16(utf8_funcname);

		string num = to_string(value_args.size());

		vector<wstring> varname_list;
		SetMacroVarAndMakeMacroKeyArray(value_args, type_args, varname_list);

		wstring arg_varname_list = StringJoin(varname_list, L", ");
		wstring utf16_expression = utf16_funcname + L" " + arg_varname_list + L";\n";

		BOOL success = CHidemaruExeExport::EvalMacro(utf16_expression);
		py::list args;
		ClearMacroVarAndUpdateArgs(varname_list, args, value_args);

		if (success) {
			return py::make_tuple(success, "", "", args);
		}
		else {
			OutputDebugStream(L"マクロの実行に失敗しました。\n");
			OutputDebugStream(L"マクロ内容:\n");
			OutputDebugStream(utf16_expression);
			return py::make_tuple(success, "", "HidemaruMacroEvalException", args);
		}
	}

	void SetMacroVarAndMakeMacroKeyArray(const pybind11::tuple &value_args, const pybind11::tuple &type_args, std::vector<std::wstring> &varname_list)
	{
		int cur_random = rand() + 1;
		for (size_t i = 0; i < value_args.size() && i < type_args.size(); i++) {
			string t = py::str(type_args[i]);
			if (t == "int") {
				wstring varname = L"#AsMacroArs_" + to_wstring(cur_random + i);
				varname_list.push_back(varname);
				string utf8_varname = utf16_to_utf8(varname);
				py::int_ o = value_args[i];
				Macro_SetVar(utf8_varname, o);
			}
			else if (t == "str") {
				wstring varname = L"$AsMacroArs_" + to_wstring(cur_random + i);
				varname_list.push_back(varname);
				string utf8_varname = utf16_to_utf8(varname);
				py::str o = value_args[i];
				Macro_SetVar(utf8_varname, o);
			}
			else if (t == "array_int") {
				wstring varname = L"#AsIntArrayOfMacroArs_" + to_wstring(cur_random + i);
				varname_list.push_back(varname);
				string utf8_varname = utf16_to_utf8(varname);

				py::tuple arr = value_args[i];
				for (size_t oix = 0; oix < arr.size(); oix++) {
					string utf8_index_varname = utf8_varname + "[" + to_string(oix) + "]";
					py::int_ o = arr[oix];
					Macro_SetVar(utf8_index_varname, o);
				}
			}
			else if (t == "array_str") {
				wstring varname = L"$AsStrArrayOfMacroArs_" + to_wstring(cur_random + i);
				varname_list.push_back(varname);
				string utf8_varname = utf16_to_utf8(varname);

				py::tuple arr = value_args[i];
				for (size_t oix = 0; oix < arr.size(); oix++) {
					string utf8_index_varname = utf8_varname + "[" + to_string(oix) + "]";
					py::str o = arr[oix];
					Macro_SetVar(utf8_index_varname, o);
				}
			}
		}
	}

	void ClearMacroVarAndUpdateArgs(std::vector<std::wstring> &varname_list, pybind11::list &args, const pybind11::tuple &value_args)
	{
		for (size_t i = 0; i < varname_list.size(); i++) {
			wstring varname = varname_list[i];
			string utf8_varname = utf16_to_utf8(varname);
			if (varname.find(L"#AsMacroArs_") != wstring::npos) {
				try {
					args.append(py::int_(Macro_GetVar(utf8_varname)));
				}
				catch (...) {
					args.append(value_args[i]);
				}
				pybind11::object o = py::int_(0);
				Macro_SetVar(utf8_varname, o);
			}
			else if (varname.find(L"$AsMacroArs_") != wstring::npos) {
				try {
					args.append(py::str(Macro_GetVar(utf8_varname)));
				}
				catch (...) {
					args.append(value_args[i]);
				}
				pybind11::str o = py::str("");
				Macro_SetVar(utf8_varname, o);
			}
			else if (varname.find(L"#AsIntArrayOfMacroArs_") != wstring::npos) {
				args.append(value_args[i]);

				py::tuple arr = value_args[i];
				for (size_t oix = 0; oix < arr.size(); oix++) {
					string utf8_index_varname = utf8_varname + "[" + to_string(oix) + "]";
					pybind11::object o = py::int_(0);
					Macro_SetVar(utf8_index_varname, o);
				}

			}
			else if (varname.find(L"$AsStrArrayOfMacroArs_") != wstring::npos) {
				args.append(value_args[i]);

				py::tuple arr = value_args[i];
				for (size_t oix = 0; oix < arr.size(); oix++) {
					string utf8_index_varname = utf8_varname + "[" + to_string(oix) + "]";
					pybind11::object o = py::str("");
					Macro_SetVar(utf8_index_varname, o);
				}
			}
		}
	}


	// アウトプット枠への出力
	BOOL OutputPane_Output(const py::object utf8_value) {
		auto str = py::str(utf8_value);
		wstring utf16_value = utf8_to_utf16(str);
		
		// ちゃんと関数がある時だけ
		if (CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle) {
			HWND hHidemaruWindow = CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle();
			if (CHidemaruExeExport::HmOutputPane_Output) {
				auto encode_byte_data = EncodeWStringToOriginalEncodeVector(utf16_value);
				BOOL result = CHidemaruExeExport::HmOutputPane_Output(hHidemaruWindow, encode_byte_data.data());
				return result;
			}
		}

		return FALSE;
	}

	// アウトプット枠表示物の一時退避
	BOOL OutputPane_Push() {

		// ちゃんと関数がある時だけ
		if (CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle) {
			HWND hHidemaruWindow = CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle();
			if (CHidemaruExeExport::HmOutputPane_Push) {
				BOOL result = CHidemaruExeExport::HmOutputPane_Push(hHidemaruWindow);
				return result;
			}
		}

		return FALSE;
	}

	// アウトプット枠表示物の一時退避してたものを戻す
	BOOL OutputPane_Pop() {

		// ちゃんと関数がある時だけ
		if (CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle) {
			HWND hHidemaruWindow = CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle();
			if (CHidemaruExeExport::HmOutputPane_Pop) {
				BOOL result = CHidemaruExeExport::HmOutputPane_Pop(hHidemaruWindow);
				return result;
			}
		}

		return FALSE;
	}


	// ハンドルの取得  (この関数はPython層へは公開していない)
	HWND OutputPane_GetWindowHanndle() {

		// ちゃんと関数がある時だけ
		if (CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle) {
			HWND hHidemaruWindow = CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle();
			if (CHidemaruExeExport::HmOutputPane_GetWindowHandle) {
				return CHidemaruExeExport::HmOutputPane_GetWindowHandle(hHidemaruWindow);
			}
		}

		return NULL;
	}

	LRESULT OutputPane_SendMessage(const int command_id) {
		HWND OutputWindowHandle = OutputPane_GetWindowHanndle();
		if (OutputWindowHandle) {
			// (#h,0x111/*WM_COMMAND*/,1009,0);//1009=クリア
			// 0x111 = WM_COMMAND
			LRESULT r = SendMessageW(OutputWindowHandle, 0x111, command_id, 0);
			return r;

		}
		return FALSE;
	}

	// SetBaseDir
	BOOL OutputPane_SetBaseDir(const std::string utf8_value) {
		wstring utf16_value = utf8_to_utf16(utf8_value);
		auto encode_byte_data = EncodeWStringToOriginalEncodeVector(utf16_value);

		// ちゃんと関数がある時だけ
		if (CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle) {
			HWND hHidemaruWindow = CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle();
			if (CHidemaruExeExport::HmOutputPane_SetBaseDir) {
				BOOL result = CHidemaruExeExport::HmOutputPane_SetBaseDir(hHidemaruWindow, encode_byte_data.data());
				return result;
			}
		}

		return FALSE;
	}

	// アウトプット枠表示物の一時退避してたものを戻す
	BOOL OutputPane_Clear() {
		// 1009 = クリア
		LRESULT r = OutputPane_SendMessage(1009);
		return (BOOL)r;
	}

	// ファイルマネージャ枠の設定
	BOOL ExplorerPane_SetMode(const int mode) {

		// ちゃんと関数がある時だけ
		if (CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle) {
			HWND hHidemaruWindow = CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle();
			if (CHidemaruExeExport::HmExplorerPane_SetMode) {
				BOOL result = CHidemaruExeExport::HmExplorerPane_SetMode(hHidemaruWindow, mode);
				return result;
			}
		}

		return FALSE;
	}

	BOOL ExplorerPane_GetMode() {

		// ちゃんと関数がある時だけ
		if (CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle) {
			HWND hHidemaruWindow = CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle();
			if (CHidemaruExeExport::HmExplorerPane_GetMode) {
				BOOL result = CHidemaruExeExport::HmExplorerPane_GetMode(hHidemaruWindow);
				return result;
			}
		}

		return FALSE;
	}

	BOOL ExplorerPane_LoadProject(const py::str utf8_value) {
		wstring utf16_value = utf8_to_utf16(utf8_value);

		// ちゃんと関数がある時だけ
		if (CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle) {
			HWND hHidemaruWindow = CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle();
			if (CHidemaruExeExport::HmExplorerPane_LoadProject) {
				auto encode_byte_data = EncodeWStringToOriginalEncodeVector(utf16_value);
				BOOL result = CHidemaruExeExport::HmExplorerPane_LoadProject(hHidemaruWindow, encode_byte_data.data());
				return result;
			}
		}

		return FALSE;
	}

	BOOL ExplorerPane_SaveProject(const py::str utf8_value) {
		wstring utf16_value = utf8_to_utf16(utf8_value);

		// ちゃんと関数がある時だけ
		if (CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle) {
			HWND hHidemaruWindow = CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle();
			if (CHidemaruExeExport::HmExplorerPane_SaveProject) {
				auto encode_byte_data = EncodeWStringToOriginalEncodeVector(utf16_value);
				BOOL result = CHidemaruExeExport::HmExplorerPane_SaveProject(hHidemaruWindow, encode_byte_data.data());
				return result;
			}
		}

		return FALSE;
	}

	// ハンドルの取得  (この関数はPython層へは公開していない)
	HWND ExplorerPane_GetWindowHanndle() {

		// ちゃんと関数がある時だけ
		if (CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle) {
			HWND hHidemaruWindow = CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle();
			if (CHidemaruExeExport::HmExplorerPane_GetWindowHandle) {
				return CHidemaruExeExport::HmExplorerPane_GetWindowHandle(hHidemaruWindow);
			}
		}

		return NULL;
	}

	LRESULT ExplorerPane_SendMessage(const int command_id) {
		HWND ExplorerWindowHandle = ExplorerPane_GetWindowHanndle();
		if (ExplorerWindowHandle) {
			//251=１つ上のフォルダ
			// 0x111 = WM_COMMAND
			LRESULT r = SendMessageW(ExplorerWindowHandle, 0x111, command_id, 0);
			return r;

		}
		return FALSE;
	}

	BOOL ExplorerPane_GetUpdated() {

		// ちゃんと関数がある時だけ
		if (CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle) {
			HWND hHidemaruWindow = CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle();
			if (CHidemaruExeExport::HmExplorerPane_GetUpdated) {
				BOOL result = CHidemaruExeExport::HmExplorerPane_GetUpdated(hHidemaruWindow);
				return result;
			}
		}

		return FALSE;
	}

	std::string ExplorerPane_GetCurrentDir() {
		if (CHidemaruExeExport::HmExplorerPane_GetCurrentDir) {
			if (Macro_IsExecuting()) {
				py::object ret = Macro_GetVar(R"RAW(dllfuncstr(loaddll("HmExplorerPane"), "GetCurrentDir", hidemaruhandle(0));)RAW");
				return py::str(ret);
			}
			else {
				py::tuple ret = Macro_Exec_EvalMemory(R"RAW(endmacro dllfuncstr(loaddll("HmExplorerPane"), "GetCurrentDir", hidemaruhandle(0));)RAW");
				py::str message = ret[1];
				return message;
			}
		}

		return "";
	}

	std::string ExplorerPane_GetProject() {
		if (CHidemaruExeExport::HmExplorerPane_GetProject) {
			if (Macro_IsExecuting()) {
				py::object ret = Macro_GetVar(R"RAW(dllfuncstr(loaddll("HmExplorerPane"), "GetProject", hidemaruhandle(0));)RAW");
				return py::str(ret);
			}
			else {
				py::tuple ret = Macro_Exec_EvalMemory(R"RAW(endmacro dllfuncstr(loaddll("HmExplorerPane"), "GetProject", hidemaruhandle(0));)RAW");
				py::str message = ret[1];
				return message;
			}
		}

		return "";
	}

	int ExplorerPane_GetProjectNative(int ch) {
		if (CHidemaruExeExport::HmExplorerPane_GetProject) {
			HWND hHidemaruWindow = CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle();
			BYTE *ret = CHidemaruExeExport::HmExplorerPane_GetProject(hHidemaruWindow);
			if (*ret != NULL) {
				ret = ret + 7;
			}

			union TYPERET {
				BYTE byte4[4];
				int a;
			};
			TYPERET typre_ret;
			typre_ret.a = 0; //　初期化

			for (int i = 0; i < 4; i++) {
				if (ret[i] == NULL) {
					break;
				}
				typre_ret.byte4[i] = ret[i];
			}

			return typre_ret.a;

				/*
			vector<BYTE> v;
			BYTE *p = ret;
			while (true) {
				if (*p) {
					v.push_back(*p);
				}
				else {
					break;
				}
				p++;
			}
			*/
		}

		return false;
	}


#pragma region
	/*
	class Job {
	public:
	std::string GetName() const { return m_name; }
	void SetName(const std::string& name) { m_name = name; }

	private:
	std::string m_name;
	};

	class Person {
	public:
	std::string GetName() const { return m_name; }
	void SetName(const std::string& name) { m_name = name; }

	std::shared_ptr<Job> GetJob() const { return m_job; }
	void SetJob(const std::shared_ptr<Job>& job) { m_job = job; }

	private:
	std::string m_name;
	std::shared_ptr<Job> m_job;
	};

	namespace py = pybind11;

	PyMODINIT_FUNC PyInit_sample() {
	py::module m("sample", "pybind11 module sample.");

	py::class_<Job, std::shared_ptr<Job>> job(m, "Job");
	job.def(py::init<>()).def_property("name", &Job::GetName, &Job::SetName);

	py::class_<Person, std::shared_ptr<Person>> person(m, "Person");
	person.def(py::init<>())
	.def_property("name", &Person::GetName, &Person::SetName)
	.def_property("job", &Person::GetJob, &Person::SetJob);

	return m.ptr();
	}
	*/
#pragma endregion 
}


PyMODINIT_FUNC PyInit_hidemaru() {
	py::module m("hidemaru", "Hidemaru python module.");
	m.attr("__version__") = (1000 * HM_PYTHON_MAJOR_VERION + 100 * HM_PYTHON_MINOR_VERION + 10 * HM_PYTHON_MICRO_VERION + HM_PYTHON_RELEASE_VERION)*0.001;
	m.def("get_version", &Hidemaru::GetVersion);
	m.def("debug_info", &Hidemaru::DebugInfo);

	py::module file = m.def_submodule("file", "Hidemaru File python module");
	file.def("get_encodingfromfile", &Hidemaru::File_GetEncodingFromFile);
	file.def("get_encodingfromhmencode", &Hidemaru::File_GetEncodingFromHmEncode);
	file.def("get_readalltext", &Hidemaru::File_ReadAllText);

	py::module edit = m.def_submodule("edit", "Hidemaru Edit python module");
	edit.def("get_filepath", &Hidemaru::Edit_GetFilePath);
	edit.def("get_totaltext", &Hidemaru::Edit_GetTotalText);
	edit.def("set_totaltext", &Hidemaru::Edit_SetTotalText);
	edit.def("get_selectedtext", &Hidemaru::Edit_GetSelectedText);
	edit.def("set_selectedtext", &Hidemaru::Edit_SetSelectedText);
	edit.def("get_linetext", &Hidemaru::Edit_GetLineText);
	edit.def("set_linetext", &Hidemaru::Edit_SetLineText);

	edit.def("get_cursorpos", &Hidemaru::Edit_GetCursorPos);
	edit.def("get_mousepos", &Hidemaru::Edit_GetCursorPosFromMousePos);

	py::module macro = m.def_submodule("macro", "Hidemaru Macro python module");
	macro.def("get_var", &Hidemaru::Macro_GetVar);
	macro.def("set_var", &Hidemaru::Macro_SetVar);
	macro.def("do_eval", &Hidemaru::Macro_Eval);
	macro.def("do_exec_eval", &Hidemaru::Macro_Exec_EvalMemory);
	macro.def("do_statement", &Hidemaru::Macro_Statement);
	macro.def("do_function", &Hidemaru::Macro_Function);
	macro.def("is_executing", &Hidemaru::Macro_IsExecuting);

	py::module outputpane = m.def_submodule("outputpane", "Hidemaru OutputPane python module");
	outputpane.def("output", &Hidemaru::OutputPane_Output);
	outputpane.def("push", &Hidemaru::OutputPane_Push);
	outputpane.def("pop", &Hidemaru::OutputPane_Pop);
	outputpane.def("clear", &Hidemaru::OutputPane_Clear);
	outputpane.def("sendmessage", &Hidemaru::OutputPane_SendMessage);
	outputpane.def("setbasedir", &Hidemaru::OutputPane_SetBaseDir);
	
	py::module explorerpane = m.def_submodule("explorerpane", "Hidemaru ExplorerPane python module");
	explorerpane.def("setmode", &Hidemaru::ExplorerPane_SetMode);
	explorerpane.def("getmode", &Hidemaru::ExplorerPane_GetMode);
	explorerpane.def("loadproject", &Hidemaru::ExplorerPane_LoadProject);
	explorerpane.def("saveproject", &Hidemaru::ExplorerPane_SaveProject);
	explorerpane.def("getwindowhandle", &Hidemaru::ExplorerPane_GetWindowHanndle);
	explorerpane.def("getupdated", &Hidemaru::ExplorerPane_GetUpdated);
	explorerpane.def("sendmessage", &Hidemaru::ExplorerPane_SendMessage);
	explorerpane.def("getproject", &Hidemaru::ExplorerPane_GetProject);
	explorerpane.def("getcurrentdir", &Hidemaru::ExplorerPane_GetCurrentDir);
	explorerpane.def("test", &Hidemaru::ExplorerPane_GetProjectNative);
	

#pragma region
	/*
	py::class_<Job, std::shared_ptr<Job>> job(m, "Job");
	job.def(py::init<>()).def_property("name", &Job::GetName, &Job::SetName);

	py::class_<Person, std::shared_ptr<Person>> person(m, "Person");
	person.def(py::init<>())
	.def_property("name", &Person::GetName, &Person::SetName)
	.def_property("job", &Person::GetJob, &Person::SetJob);
	*/
#pragma endregion 

	return m.ptr();
}

