/* 
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#include "python_hidemaru_lib.h"
#include "pybind11.h"

using namespace pybind11::literals;

namespace Hidemaru {

	// バージョンの取得
	double GetVersion() {
		return CHidemaruExeExport::hm_version;
	}

	// オブジェクトとして捉え、文字列に変換して出力
	void DebugInfo(const py::object message) {
		auto str = py::str(message);
		OutputDebugStream(utf8_to_utf16(str));
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
	
	std::string File_GetEncodingAlias(const std::string utf8_filename) {
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

		int hm_encode = File_GetHmEncode(utf8_filename);

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
		if (hm_encode <= 0) {
			return "";
		}
		if (hm_encode >= code_arr_length) {
			return "";
		}

		int codepage = code_arr[hm_encode];
		switch (codepage) {
		case 0: {
			return "";
		}
		case 932: {
			return "cp932";
		}
		case 1200: {
			return "utf_16_le";
		}
		case 51932: {
			return "euc_jp";
		}
		case 50221: {
			return "iso2022_jp";
		}
		case 65000: {
			return "utf_7";
		}
		case 65001: {
			return "utf_8";
		}
		case 1201: {
			return "utf_16_be";
		}
		case 1252: {
			return "cp1252";
		}
		case 936: {
			return "gb2312";
		}
		case 950: {
			return "big5";
		}
		case 949: {
			return "cp949";
		}
		case 1361: {
			return "cp1361";
		}
		case 1250: {
			return "cp1250";
		}
		case 1257: {
			return "cp1257";
		}
		case 1253: {
			return "cp1253";
		}
		case 1251: {
			return "cp1251";
		}
		case 42: {
			return "";
		}
		case 1254: {
			return "cp1254";
		}
		case 1255: {
			return "cp1255";
		}
		case 1256: {
			return "cp1256";
		}
		case 874: {
			return "iso8859_15";
		}
		case 1258: {
			return "cp1258";
		}
		case 10001: {
			return "x_mac_japanese";
		}
		case 850: {
			return "cp850";
		}
		case 12000: {
			return "utf_32_le";
		}
		case 12001: {
			return "utf_32_be";
		}
		}
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

	m.def("get_version", &Hidemaru::GetVersion);
	m.def("debug_info", &Hidemaru::DebugInfo);

	py::module edit = m.def_submodule("edit", "Hidemaru Edit python module");
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

