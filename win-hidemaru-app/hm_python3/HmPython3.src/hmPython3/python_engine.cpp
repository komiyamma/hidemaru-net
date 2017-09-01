
#include "python_hidemaru_lib.h"


static wchar_t* program = nullptr;
namespace PythonEngine {

	int Create()
	{
		wchar_t szHidemaruFullPath[MAX_PATH];
		GetModuleFileName(NULL, szHidemaruFullPath, _countof(szHidemaruFullPath));

		wchar_t python_home[] = LR"(c:\usr\cpython3)";

		try {
			Py_SetPythonHome(python_home);
			program = Py_DecodeLocale(utf16_to_utf8(szHidemaruFullPath).c_str(), nullptr);
			if (program == nullptr) {
				MessageBox(NULL, L"Fatal error: cannot decode", L"Fatal error: cannot decode", NULL);
				fprintf(stderr, "Fatal error: cannot decode argv[0]\n");
				return FALSE;
				// exit(1);
			}

			PyImport_AppendInittab("hm", PyInit_hidemaru);

			Py_SetProgramName(program);

			Py_Initialize();

			//	py::module::import("hm");
			py::eval<py::eval_single_statement>("import sys");
			py::eval<py::eval_single_statement>("import hm");
			py::eval<py::eval_single_statement>("sys.dont_write_bytecode = True");
			auto global = py::dict(py::module::import("__main__").attr("__dict__"));
			py::eval<py::eval_statements>(
				"def DestroyScope():\n"
				"    pass");

			return TRUE;
		}
		catch (py::error_already_set& e) {
			OutputDebugStream(L"エラー:\n" + utf8_to_utf16(e.what()));
		}

		return FALSE;
	}

	int DoString(string utf8expression) {

		try {
			/*
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
			*/

			py::eval<py::eval_statements>(utf8expression);

			return TRUE;
		}
		catch (py::error_already_set& e) {
			OutputDebugStream(L"エラー:\n" + utf8_to_utf16(e.what()));
		}

		return FALSE;
	}


	int Destroy() {

		if (program) {

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
				PyMem_RawFree(program);
			}
			catch (py::error_already_set& e) {
				OutputDebugStream(L"エラー:\n" + utf8_to_utf16(e.what()));
			}
		}
		program = nullptr;

		return TRUE;
	}

}