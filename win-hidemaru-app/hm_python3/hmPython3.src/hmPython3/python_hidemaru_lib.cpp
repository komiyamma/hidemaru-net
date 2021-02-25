/* 
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#include "python_hidemaru_lib.h"
#include "pybind11.h"

using namespace pybind11::literals;

extern vector<BYTE> EncodeWStringToOriginalEncodeVector(wstring original_string);


namespace Hidemaru {

	// �o�[�W�����̎擾
	double GetVersion() {
		return CHidemaruExeExport::hm_version;
	}

	// �I�u�W�F�N�g�Ƃ��đ����A������ɕϊ����ďo��
	void DebugInfo(const py::object message) {
		auto str = py::str(message);
		OutputDebugStream(utf8_to_utf16(str));
	}

	// �G�ۂŌ��ݕҏW���Ă���e�L�X�g�S�̂̎擾
	std::string Edit_GetTotalText() {
		wstring utf16_value = CHidemaruExeExport::GetTotalText();
		return utf16_to_utf8(utf16_value);
	}

	// �G�ۂŌ��ݕҏW���Ă���e�L�X�g�S�̂̐ݒ�
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

	// �G�ۂŌ��ݕҏW���Ă���I���e�L�X�g�̎擾
	std::string Edit_GetSelectedText() {
		wstring utf16_value = CHidemaruExeExport::GetSelectedText();
		return utf16_to_utf8(utf16_value);
	}

	// �G�ۂŌ��ݕҏW���Ă���I���e�L�X�g�̐ݒ�
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

	// �G�ۂŌ��ݕҏW���Ă���e�L�X�g�̒��ŁA�J�[�\��������s�̓��e���擾
	std::string Edit_GetLineText() {
		wstring utf16_value = CHidemaruExeExport::GetLineText();
		return utf16_to_utf8(utf16_value);
	}

	// �G�ۂŌ��ݕҏW���Ă���e�L�X�g�̒��ŁA�J�[�\��������s�̓��e��ݒ�
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
	// �w��Ώۂ̃t�@�C���̏G��encode�𓾂�B�G�ێ��̂́u������v-�u�t�@�C���v�[�u�G���R�[�h1�v�̐ݒ�ɏ]���̂Œ��ӁB
	// �ėp�̃G���R�[�h�擾�@�\�ł͂Ȃ��A�g�p�҂��g���Ă���G�ۂ̐ݒ�̌��A���̊Y���̏G�ۂŃt�@�C�����J�����Ƃ�����A
	// ���̃G���R�[�h�ŊJ�����̂����擾����@�\�ƂȂ�B
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
		1252,   // encode = 8 ���� ANSI Latin 1; Western European (Windows)
		936,    // encode = 9 �ȑ̎������� ANSI/OEM Simplified Chinese (PRC, Singapore); Chinese Simplified (GB2312)
		950,    // encode =10 �ɑ̎������� ANSI/OEM Traditional Chinese (Taiwan; Hong Kong SAR, PRC); Chinese Traditional (Big5)
		949,    // encode =11 �؍��� ANSI/OEM Korean (Unified Hangul Code)
		1361,   // encode =12 �؍��� Korean (Johab)
		1250,   // encode =13 �������[���b�p���� ANSI Central European; Central European (Windows)
		1257,   // encode =14 �o���g�� ANSI Baltic; Baltic (Windows)
		1253,   // encode =15 �M���V���� ANSI Greek; Greek (Windows)
		1251,   // encode =16 �L�������� ANSI Cyrillic; Cyrillic (Windows)
		42,     // encode =17 �V���{��
		1254,   // encode =18 �g���R�� ANSI Turkish; Turkish (Windows)
		1255,   // encode =19 �w�u���C�� ANSI Hebrew; Hebrew (Windows)
		1256,   // encode =20 �A���r�A�� ANSI Arabic; Arabic (Windows)
		874,    // encode =21 �^�C�� ANSI/OEM Thai (same as 28605, ISO 8859-15); Thai (Windows)
		1258,   // encode =22 �x�g�i���� ANSI/OEM Vietnamese; Vietnamese (Windows)
		10001,  // encode =23 x-mac-japanese Japanese (Mac)
		850,    // encode =24 OEM/DOS
		0,      // encode =25 ���̑�
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

		// �R�[�h�y�[�W�̈ꗗ�\�̌�
		const int code_arr_length = sizeof(code_arr) / sizeof(int);

		// �͈̓`�F�b�N(�G�ۂ��Ԃ���encode�l�������͂ݏo���Ă�����...)
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

	// �w��̕����R�[�h���J��
	py::tuple  File_ReadAllText(const std::string utf8_filename, int hm_encode) {
		wstring utf16_filename = utf8_to_utf16(utf8_filename);
		UINT encoded_character_count = 0;
		bool success = 0;
		wstring utf16_value = CHidemaruExeExport::LoadFileUnicode(utf16_filename, hm_encode, &encoded_character_count, NULL, NULL, &success);
		return py::make_tuple(success, utf16_to_utf8(utf16_value));
	}

	// �J�[�\���̈ʒu�B�����₷���̂�python�̃^�v��(lineno, column)�ŕԂ��Ă���
	py::tuple Edit_GetCursorPos() {
		auto ret = CHidemaruExeExport::GetCursorPos();
		return py::make_tuple(ret.lineno, ret.column);
	}

	// �}�E�X�̍��W�̉��ɑ�������(lineno, column)�̒l�B�����₷���̂�python�̃^�v��(lineno, column, x, y)�ŕԂ��Ă���
	// x, y�͖{����win32api�ł̃}�E�X�̍��W
	py::tuple Edit_GetCursorPosFromMousePos() {
		auto ret = CHidemaruExeExport::GetCursorPosFromMousePos();
		return py::make_tuple(ret.lineno, ret.column, ret.x, ret.y);
	}

	// �V���{���̒l�̎擾
	py::object Macro_GetVar(const std::string utf8_simbol) {

		wstring utf16_simbol = utf8_to_utf16(utf8_simbol);

		TestDynamicVar.Clear();
		auto dll_invocant = CSelfDllInfo::GetInvocantString();
		wstring cmd =
			L"##_tmp_dll_id_ret = dllfuncw( " + dll_invocant + L"\"SetDynamicVar\", " + utf16_simbol + L");\n"
			L"##_tmp_dll_id_ret = 0;\n";
		BOOL success = CHidemaruExeExport::EvalMacro(cmd);

		// ���l�Ȃ�
		if (TestDynamicVar.type == CDynamicValue::TDynamicType::TypeInteger)
		{
			py::object r = py::cast(TestDynamicVar.num);
			return r;
		}
		// ������Ȃ�
		else {
			string utf8_value = utf16_to_utf8(TestDynamicVar.wstr);
			py::object r = py::cast(utf8_value);
			return r;
		}
	}

	// �V���{���̒l�̐ݒ�
	BOOL Macro_SetVar(const std::string utf8_simbol, const py::object value) {
		BOOL success = 0;

		auto dll_invocant = CSelfDllInfo::GetInvocantString();

		wstring utf16_simbol = utf8_to_utf16(utf8_simbol);
		wchar_t start = utf16_simbol[0];
		if (start == L'#') {

			string utf8_value = py::str(value);
			wstring utf16_value = utf8_to_utf16(utf8_value);

			// �����𐔒l�Ƀg���C�B�_���Ȃ�0����B
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

	// python�̒�����G�ۃ}�N������������s
	py::tuple Macro_Eval(const std::string utf8_expression) {
		wstring utf16_expression = utf8_to_utf16(utf8_expression);

		BOOL success = CHidemaruExeExport::EvalMacro(utf16_expression);
		if (success) {
			return py::make_tuple(success, "", "");
		}
		else {
			OutputDebugStream(L"�}�N���̎��s�Ɏ��s���܂����B\n");
			OutputDebugStream(L"�}�N�����e:\n");
			OutputDebugStream(utf16_expression);
			return py::make_tuple( success, "", "HidemaruMacroEvalException");
		}
	}



	// �A�E�g�v�b�g�g�ւ̏o��
	BOOL OutputPane_Output(const std::string utf8_value) {
		wstring utf16_value = utf8_to_utf16(utf8_value);
		auto encode_byte_data = EncodeWStringToOriginalEncodeVector(utf16_value);
		
		// �����Ɗ֐������鎞����
		if (CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle) {
			HWND hHidemaruWindow = CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle();
			if (CHidemaruExeExport::HmOutputPane_Output) {
				BOOL result = CHidemaruExeExport::HmOutputPane_Output(hHidemaruWindow, encode_byte_data.data());
				return result;
			}
		}

		return FALSE;
	}

	// �A�E�g�v�b�g�g�\�����̈ꎞ�ޔ�
	BOOL OutputPane_Push() {

		// �����Ɗ֐������鎞����
		if (CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle) {
			HWND hHidemaruWindow = CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle();
			if (CHidemaruExeExport::HmOutputPane_Push) {
				BOOL result = CHidemaruExeExport::HmOutputPane_Push(hHidemaruWindow);
				return result;
			}
		}

		return FALSE;
	}

	// �A�E�g�v�b�g�g�\�����̈ꎞ�ޔ����Ă����̂�߂�
	BOOL OutputPane_Pop() {

		// �����Ɗ֐������鎞����
		if (CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle) {
			HWND hHidemaruWindow = CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle();
			if (CHidemaruExeExport::HmOutputPane_Pop) {
				BOOL result = CHidemaruExeExport::HmOutputPane_Pop(hHidemaruWindow);
				return result;
			}
		}

		return FALSE;
	}

	// �n���h���̎擾  (���̊֐���Python�w�ւ͌��J���Ă��Ȃ�)
	HWND OutputPane_GetWindowHanndle() {

		// �����Ɗ֐������鎞����
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
			// (#h,0x111/*WM_COMMAND*/,1009,0);//1009=�N���A
			// 0x111 = WM_COMMAND
			LRESULT r = SendMessageW(OutputWindowHandle, 0x111, command_id, 0);
			return r;

		}
		return FALSE;
	}

	// �A�E�g�v�b�g�g�ւ̏o��
	BOOL OutputPane_SetBaseDir(const std::string utf8_value) {
		wstring utf16_value = utf8_to_utf16(utf8_value);
		auto encode_byte_data = EncodeWStringToOriginalEncodeVector(utf16_value);

		// �����Ɗ֐������鎞����
		if (CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle) {
			HWND hHidemaruWindow = CHidemaruExeExport::Hidemaru_GetCurrentWindowHandle();
			if (CHidemaruExeExport::HmOutputPane_SetBaseDir) {
				BOOL result = CHidemaruExeExport::HmOutputPane_SetBaseDir(hHidemaruWindow, encode_byte_data.data());
				return result;
			}
		}

		return FALSE;
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

	py::module file = m.def_submodule("file", "Hidemaru File python module");
	file.def("get_encodingfromfile", &Hidemaru::File_GetEncodingFromFile);
	file.def("get_encodingfromhmencode", &Hidemaru::File_GetEncodingFromHmEncode);
	file.def("get_readalltext", &Hidemaru::File_ReadAllText);

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

	py::module outputpane = m.def_submodule("outputpane", "Hidemaru OutputPane python module");
	outputpane.def("output", &Hidemaru::OutputPane_Output);
	outputpane.def("push", &Hidemaru::OutputPane_Push);
	outputpane.def("pop", &Hidemaru::OutputPane_Pop);
	outputpane.def("sendmessage", &Hidemaru::OutputPane_SendMessage);
	outputpane.def("setbasedir", &Hidemaru::OutputPane_SetBaseDir);
	

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

