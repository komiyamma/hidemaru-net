
#include "pybind11/eval.h"
#include "pybind11/embed.h"

#include "convert_string.h"
#include "output_debugstream.h"
#include "hidemaruexe_export.h"
#include "self_dll_info.h"
#include "dllfunc_interface_internal.h"

#include <iostream>
#include <windows.h>
#include <string>

namespace py = pybind11;

static BOOL SetTotalText(wstring utf16_value) {
}


namespace Hidemaru {

	// オブジェクトとして捉え、文字列に変換して出力
	void debuginfo(const py::object message) {
		auto str = py::str(message);
		OutputDebugStream(utf8_to_utf16(str));
	}

	class Edit {

	public:
		static std::string GetTotalText() {
			wstring utf16_value = CHidemaruExeExport::GetTotalText();
			return utf16_to_utf8(utf16_value);
		
		}
		static void SetTotalText(const std::string& utf8_value) {
			
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
			
			// return success;
		}
	};

	class Macro {
	public:
		std::string GetName() const { return m_name; }
		void SetName(const std::string& name) {
		
		}

		std::shared_ptr<Edit> GetJob() const { return m_job; }
		void SetJob(const std::shared_ptr<Edit>& job) { m_job = job; }

	private:
		std::string m_name;
		std::shared_ptr<Edit> m_job;
	};
}

PyMODINIT_FUNC PyInit_hidemaru() {
	py::module m("hm", "Hidemaru python module.");

	py::class_<Hidemaru::Edit, std::shared_ptr<Hidemaru::Edit>> job(m, "Edit");
	job.def(py::init<>()).def_property_static("TotalText", &Hidemaru::Edit::GetTotalText, &Hidemaru::Edit::SetTotalText);

	py::class_<Hidemaru::Macro, std::shared_ptr<Hidemaru::Macro>> person(m, "Macro");
	person.def(py::init<>())
		.def_property("name", &Hidemaru::Macro::GetName, &Hidemaru::Macro::SetName)
		.def_property("job", &Hidemaru::Macro::GetJob, &Hidemaru::Macro::SetJob);

	m.def("debuginfo", &Hidemaru::debuginfo);

	return m.ptr();
}

static wchar_t* program = nullptr;
namespace PythonEngine {

	int Create()
	{
		wchar_t python_home[] = LR"(c:\usr\cpython3)";

		try {
			Py_SetPythonHome(python_home);
			wchar_t exename[MAX_PATH];
			GetModuleFileName(NULL, exename, _countof(exename));
			program = Py_DecodeLocale(utf16_to_utf8(exename).c_str(), nullptr);
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

			return TRUE;
		}
		catch (py::error_already_set& e) {
			std::cout << "Python error.エラー\n" << e.what() << "\n";
			OutputDebugString(L"エラー:\n");
			OutputDebugString(utf8_to_utf16(e.what()).c_str());
			OutputDebugString(L"\n\n");
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
			std::cout << "Python error.エラー\n" << e.what() << "\n";
			OutputDebugString(L"エラー:\n");
			OutputDebugString(utf8_to_utf16(e.what()).c_str());
			OutputDebugString(L"\n\n");
		}

		return FALSE;
	}


	int Destroy() {

		if (program) {

			try {
				Py_Finalize();
				PyMem_RawFree(program);
			}
			catch (py::error_already_set& e) {
				std::cout << "Python error.エラー\n" << e.what() << "\n";
				OutputDebugString(L"エラー:\n");
				OutputDebugString(utf8_to_utf16(e.what()).c_str());
				OutputDebugString(L"\n\n");
			}
		}
		program = nullptr;

		return TRUE;
	}

}