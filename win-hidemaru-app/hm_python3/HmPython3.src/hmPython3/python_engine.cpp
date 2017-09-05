
#include "python_hidemaru_lib.h"
#include "python_engine.h"

namespace PythonEngine {

	wchar_t* m_wszProgram = nullptr;
	BOOL m_isValid = false;
	BOOL IsValid() {
		return m_isValid;
	}

	int Create()
	{
		m_isValid = FALSE;
		wchar_t szHidemaruFullPath[MAX_PATH];
		GetModuleFileName(NULL, szHidemaruFullPath, _countof(szHidemaruFullPath));

		wstring python_home = GetPythonPath();
		if (python_home.size() == 0) {
			return FALSE;
		}

		try {
			Py_SetPythonHome((wchar_t *)python_home.data());
			m_wszProgram = Py_DecodeLocale(utf16_to_utf8(szHidemaruFullPath).c_str(), nullptr);
			if (m_wszProgram == nullptr) {
				MessageBox(NULL, L"致命的エラー: 秀丸自身のフルパス名を解釈出来ません", L"致命的エラー: 秀丸自身のフルパス名を解釈出来ません", NULL);
				return FALSE;
			}

			PyImport_AppendInittab("hidemaru", PyInit_hidemaru);

			Py_SetProgramName(m_wszProgram);

			Py_Initialize();

			//	py::module::import("hidemaru");
			py::eval<py::eval_single_statement>("import sys");
			py::eval<py::eval_single_statement>("import hidemaru");
			py::eval<py::eval_single_statement>("sys.dont_write_bytecode = True");
			auto global = py::dict(py::module::import("__main__").attr("__dict__"));
			
			py::eval<py::eval_statements>(
				"def DestroyScope():\n"
				"    pass");

			m_isValid = TRUE;
			return TRUE;
		}
		catch (py::error_already_set& e) {
			OutputDebugStream(L"エラー:\n" + utf8_to_utf16(e.what()));
		}

		return FALSE;
	}

	static bool m_isInitialize = FALSE;
	int Initialize() {
		if (!m_isInitialize) {
			auto global = py::dict(py::module::import("__main__").attr("__dict__"));
			auto local = py::dict();
			// マクロを呼び出した元のフォルダはpythonファイルの置き場としても認識する
			py::eval<py::eval_single_statement>("sys.path.append( hidemaru.macro.get_var('currentmacrodirectory') )");

			m_isInitialize = TRUE;
		}
		return true;
	}

	intHM_t GetNumVar(wstring utf16_simbol) {
		try {
			auto global = py::dict(py::module::import("__main__").attr("__dict__"));
			auto local = py::dict();

			string utf8_simbol = utf16_to_utf8(utf16_simbol);
			auto value = global[utf8_simbol.c_str()];

			string utf8_value = py::str(value);
			wstring utf16_value = utf8_to_utf16(utf8_value);

			// 数字を数値にトライ。ダメなら0だよ。
			intHM_t n = 0;
			try {
				n = (intHM_t)std::stol(utf16_value);
			}
			catch (...) {
				n = 0;
			}
			return n;
		}
		catch (py::error_already_set& e) {
			OutputDebugStream(L"エラー:\n" + utf8_to_utf16(e.what()));
		}

		return 0;
	}

	BOOL SetNumVar(wstring utf16_simbol, intHM_t value) {
		try {
			auto global = py::dict(py::module::import("__main__").attr("__dict__"));
			auto local = py::dict();

			string utf8_simbol = utf16_to_utf8(utf16_simbol);
			global[utf8_simbol.c_str()] = value;

			return TRUE;
		}
		catch (py::error_already_set& e) {
			OutputDebugStream(L"エラー:\n" + utf8_to_utf16(e.what()));
		}

		return FALSE;
	}

	wstring GetStrVar(wstring utf16_simbol) {
		try {
			auto global = py::dict(py::module::import("__main__").attr("__dict__"));
			auto local = py::dict();

			string utf8_simbol = utf16_to_utf8(utf16_simbol);
			auto value = global[utf8_simbol.c_str()];

			string utf8_value = py::str(value);

			wstring utf16_value = utf8_to_utf16(utf8_value);
			return utf16_value;
		}
		catch (py::error_already_set& e) {
			OutputDebugStream(L"エラー:\n" + utf8_to_utf16(e.what()));
		}

		return L"";
	}

	BOOL SetStrVar(wstring utf16_simbol, wstring utf16_value) {
		try {
			auto global = py::dict(py::module::import("__main__").attr("__dict__"));
			auto local = py::dict();

			string utf8_simbol = utf16_to_utf8(utf16_simbol);
			string utf8_value = utf16_to_utf8(utf16_value);
			global[utf8_simbol.c_str()] = utf8_value;

			return TRUE;
		}
		catch (py::error_already_set& e) {
			OutputDebugStream(L"エラー:\n" + utf8_to_utf16(e.what()));
		}

		return FALSE;
	}


	/*
	int Test() {
		auto global = py::dict(py::module::import("__main__").attr("__dict__"));
		auto local = py::dict();

		local["x"] = 100;
		py::eval<py::eval_single_statement>("y = 200", global, local);
		py::eval("print('x + y =', x + y)", global, local); // x + y = 300
		auto z = local["y"].cast<int>();
		printf("%d", z);
		OutputDebugString(std::to_wstring(z).data());

		py::eval<py::eval_statements>("import random\nrandom.random()");
		py::eval<py::eval_single_statement>(utf16_to_utf8(L"print('あああ')"));
		// Python で関数を作成 (Name=Hoge, Job=Teacher)
		py::eval<py::eval_statements>(
		"def initialize_person(p):\n"
		"   job = hm.Edit()\n"
		"   job.name = 'Teacher'\n"
		"   p.name = 'Hoge'\n"
		"   p.job = job\n",
		global);
		{
		auto person = std::make_shared<Hidemaru::Macro>();
		global["initialize_person"](person);
		std::cout << "Name : " << person->GetName() << "\n";           // Name : Hoge
		std::cout << "Job  : " << person->GetJob()->GetName() << "\n"; // Job  : Teacher
		}
	}
	*/
	int DoString(wstring utf16_expression) {

		try {

			string utf8_expression = utf16_to_utf8(utf16_expression);
			py::eval<py::eval_statements>(utf8_expression);

			return TRUE;
		}
		catch (py::error_already_set& e) {
			OutputDebugStream(L"エラー:\n" + utf8_to_utf16(e.what()));
		}

		return FALSE;
	}


	int Destroy() {

		if (m_wszProgram) {

			try {
				auto global = py::dict(py::module::import("__main__").attr("__dict__"));
				auto func = global["DestroyScope"];
				func();
			}
			catch (py::error_already_set& e) {
				OutputDebugStream(L"エラー:\n" + utf8_to_utf16(e.what()));
			}

			try {
				Py_Finalize();
				PyMem_RawFree(m_wszProgram);
			}
			catch (py::error_already_set& e) {
				OutputDebugStream(L"エラー:\n" + utf8_to_utf16(e.what()));
			}
		}
		m_wszProgram = nullptr;

		m_isValid = FALSE;

		m_isInitialize = FALSE;

		return TRUE;
	}

}