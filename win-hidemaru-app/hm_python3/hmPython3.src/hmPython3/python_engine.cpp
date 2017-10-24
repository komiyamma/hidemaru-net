
#include "python_hidemaru_lib.h"
#include "python_engine.h"
#include "exception_translator.h"


namespace PythonEngine {

	// エンジンが有効になった
	BOOL m_isValid = false;
	// 秀丸の名前格納
	wchar_t *m_wstr_program = NULL;

	wchar_t szHidemaruFullPath[MAX_PATH] = L"";

	BOOL IsValid() {
		return m_isValid;
	}

	// エンジン生成
	int _Create()
	{
		m_isValid = FALSE;

		// XMLの設定に従ってPythonのパスを取得
		wstring python_home = GetPythonPath();
		if (python_home.size() == 0) {
			return FALSE;
		}

		try {
			// パイソンのホーム設定
			Py_SetPythonHome((wchar_t *)python_home.data());

			// ビルトインとして定義する
			PyImport_AppendInittab("hidemaru", PyInit_hidemaru);

			GetModuleFileName(NULL, szHidemaruFullPath, _countof(szHidemaruFullPath));

			// 自分の名前の伝達。(相対パスの解釈のために利用されると思われる)
			m_wstr_program = szHidemaruFullPath;
			Py_SetProgramName(m_wstr_program);

			// 決まり文句
			py::initialize_interpreter();
			{
				//	py::module::import("hidemaru");
				// キャッシュを作らないためにsysのインポート。
				py::eval<py::eval_single_statement>("import sys");
				// 秀丸用のクラスのインポート。他で、これ経由で処理したい場合などに利用する
				py::eval<py::eval_single_statement>("import hidemaru");
				// pythonライブラリなどを使用した時にキャッシュを迂闊に作らないように。(そうでないとあちこちにキャッシュが出来る)
				py::eval<py::eval_single_statement>("sys.dont_write_bytecode = True");

				// メイン辞書読み込み
				auto global = py::dict(py::module::import("__main__").attr("__dict__"));
			}
#pragma region
			/*
			// DestroyScopeの定義
			py::eval<py::eval_statements>(
				"def DestroyScope():\n"
				"    pass");
			*/
#pragma endregion
			// ここまで実行出来たら、エンジンとして有効になったというフラグが立つ
			m_isValid = TRUE;
			return TRUE;
		}
		catch (py::error_already_set& e) {
			PythonAlreadySetException(e);
		}
		catch (exception& e) {
			PythonKnknownException(e);
		}
		catch (CSEHException &e) {
			SystemAlreadySetException(e);
		}
		catch (...) {
			SystemUnknownException();
		}

		// エンジンとして駄目
		return FALSE;
	}

	int Create() {
		auto prevTransEHtoCEH = _set_se_translator(PythonTransSEHtoCEH);
		auto ret = _Create();
		_set_se_translator(prevTransEHtoCEH);
		return ret;
	}


	// エンジンが構築された後に１回だけ実行するように
	static bool m_isInitialize = FALSE;
	int _Initialize() {
		if (!IsValid()) {
			return FALSE;
		}
		try {
			if (!m_isInitialize) {
				auto global = py::dict(py::module::import("__main__").attr("__dict__"));
				auto local = py::dict();
				// マクロを呼び出した元のフォルダはpythonファイルの置き場としても認識する
				py::eval<py::eval_single_statement>("sys.path.append( hidemaru.macro.get_var('currentmacrodirectory') )");

				m_isInitialize = TRUE;
			}
		}
		catch (py::error_already_set& e) {
			PythonAlreadySetException(e);
		}
		catch (exception& e) {
			PythonKnknownException(e);
		}
		catch (CSEHException &e) {
			SystemAlreadySetException(e);
		}
		catch (...) {
			SystemUnknownException();
		}

		return TRUE;
	}

	int Initialize() {
		auto prevTransEHtoCEH = _set_se_translator(PythonTransSEHtoCEH);
		auto ret = _Initialize();
		_set_se_translator(prevTransEHtoCEH);
		return ret;
	}

	// 対象のシンボル名の値を数値として得る
	intHM_t _GetNumVar(wstring utf16_simbol) {
		try {
			auto global = py::dict(py::module::import("__main__").attr("__dict__"));
			auto local = py::dict();

			// 値を得て…
			string utf8_simbol = utf16_to_utf8(utf16_simbol);
			auto value = global[utf8_simbol.c_str()];

			// 文字列化するのは、実態がなんのオブジェクトかわからないため
			string utf8_value = py::str(value);
			wstring utf16_value = utf8_to_utf16(utf8_value);

			// 数字を数値に変換トライ。ダメなら0だよ。
			intHM_t n = 0;
			try {
				n = (intHM_t)std::stoll(utf16_value);
			}
			catch (...) {
				n = 0;
			}
			return n;
		}
		catch (py::error_already_set& e) {
			PythonAlreadySetException(e);
		}
		catch (exception& e) {
			PythonKnknownException(e);
		}
		catch (CSEHException &e) {
			SystemAlreadySetException(e);
		}
		catch (...) {
			SystemUnknownException();
		}

		return 0;
	}

	intHM_t GetNumVar(wstring utf16_simbol) {
		auto prevTransEHtoCEH = _set_se_translator(PythonTransSEHtoCEH);
		auto ret = _GetNumVar(utf16_simbol);
		_set_se_translator(prevTransEHtoCEH);
		return ret;
	}


	// 対象のシンボル名の値に数値を代入する
	BOOL _SetNumVar(wstring utf16_simbol, intHM_t value) {
		try {
			auto global = py::dict(py::module::import("__main__").attr("__dict__"));
			auto local = py::dict();

			// 値を代入
			string utf8_simbol = utf16_to_utf8(utf16_simbol);
			global[utf8_simbol.c_str()] = value;

			return TRUE;
		}
		catch (py::error_already_set& e) {
			PythonAlreadySetException(e);
		}
		catch (exception& e) {
			PythonKnknownException(e);
		}
		catch (CSEHException &e) {
			SystemAlreadySetException(e);
		}
		catch (...) {
			SystemUnknownException();
		}

		return FALSE;
	}

	BOOL SetNumVar(wstring utf16_simbol, intHM_t value) {
		auto prevTransEHtoCEH = _set_se_translator(PythonTransSEHtoCEH);
		auto ret = _SetNumVar(utf16_simbol, value);
		_set_se_translator(prevTransEHtoCEH);
		return ret;
	}

	// 対象のシンボル名の値を文字列として得る
	wstring _GetStrVar(wstring utf16_simbol) {
		try {
			auto global = py::dict(py::module::import("__main__").attr("__dict__"));
			auto local = py::dict();

			// 値を得て…
			string utf8_simbol = utf16_to_utf8(utf16_simbol);
			auto value = global[utf8_simbol.c_str()];

			// 文字列化して
			string utf8_value = py::str(value);

			wstring utf16_value = utf8_to_utf16(utf8_value);
			return utf16_value;
		}
		catch (py::error_already_set& e) {
			PythonAlreadySetException(e);
		}
		catch (exception& e) {
			PythonKnknownException(e);
		}
		catch (CSEHException &e) {
			SystemAlreadySetException(e);
		}
		catch (...) {
			SystemUnknownException();
		}

		return L"";
	}

	wstring GetStrVar(wstring utf16_simbol) {
		auto prevTransEHtoCEH = _set_se_translator(PythonTransSEHtoCEH);
		auto ret = _GetStrVar(utf16_simbol);
		_set_se_translator(prevTransEHtoCEH);
		return ret;
	}

	// 対象のシンボル名の値に文字列を代入する
	BOOL _SetStrVar(wstring utf16_simbol, wstring utf16_value) {
		try {
			auto global = py::dict(py::module::import("__main__").attr("__dict__"));
			auto local = py::dict();

			// 値を代入
			string utf8_simbol = utf16_to_utf8(utf16_simbol);
			string utf8_value = utf16_to_utf8(utf16_value);
			global[utf8_simbol.c_str()] = utf8_value;

			return TRUE;
		}
		catch (py::error_already_set& e) {
			PythonAlreadySetException(e);
		}
		catch (exception& e) {
			PythonKnknownException(e);
		}
		catch (CSEHException &e) {
			SystemAlreadySetException(e);
		}
		catch (...) {
			SystemUnknownException();
		}

		return FALSE;
	}

	BOOL SetStrVar(wstring utf16_simbol, wstring utf16_value) {
		auto prevTransEHtoCEH = _set_se_translator(PythonTransSEHtoCEH);
		auto ret = _SetStrVar(utf16_simbol, utf16_value);
		_set_se_translator(prevTransEHtoCEH);
		return ret;
	}

#pragma region
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
#pragma endregion

	// 対象の文字列をPythonの複数式とみなして評価する
	int _DoString(wstring utf16_expression) {
		if (!IsValid()) {
			return FALSE;
		}

		try {

			string utf8_expression = utf16_to_utf8(utf16_expression);

			py::eval<py::eval_statements>(utf8_expression);

			return TRUE;
		}
		catch (py::error_already_set& e) {
			PythonAlreadySetException(e);
		}
		catch (exception& e) {
			PythonKnknownException(e);
		}
		catch (CSEHException &e) {
			SystemAlreadySetException(e);
		}
		catch (...) {
			SystemUnknownException();
		}

		return FALSE;
	}

	int DoString(wstring utf16_expression) {
		auto prevTransEHtoCEH = _set_se_translator(PythonTransSEHtoCEH);
		auto ret = _DoString(utf16_expression);
		_set_se_translator(prevTransEHtoCEH);
		return ret;
	}

	// エンジンの破棄
	int _Destroy() {

		// 有効でないならば、即終了
		if (!IsValid()) {
			return FALSE;
		}

		// 有効な時だけ
		if (m_isValid) {

			try {
				// DestroyScopeというのが、メインモジュール内に定義されていれば、それを実行する
				auto global = py::dict(py::module::import("__main__").attr("__dict__"));

				// マクロを呼び出した元のフォルダはpythonファイルの置き場としても認識する
				py::eval<py::eval_statements>("if 'DestroyScope' in globals(): DestroyScope()");
#pragma region
				/*
				auto func = global["DestroyScope"];
				func();
				*/
#pragma endregion
			}
			catch (py::error_already_set& e) {
				PythonAlreadySetException(e);
			}
			catch (exception& e) {
				PythonKnknownException(e);
			}
			catch (CSEHException &e) {
				SystemAlreadySetException(e);
			}
			catch (...) {
				SystemUnknownException();
			}

			// 破棄
			try {
				py::finalize_interpreter();

				// PyMem_RawFree(m_wstr_program);
			}
			catch (py::error_already_set& e) {
				PythonAlreadySetException(e);
			}
			catch (exception& e) {
				PythonKnknownException(e);
			}
			catch (CSEHException &e) {
				SystemAlreadySetException(e);
			}
			catch (...) {
				SystemUnknownException();
			}
		}

		// 初期状態へ
		m_isValid = FALSE;
		m_isInitialize = FALSE;
		// m_wstr_program = NULL;

		return TRUE;
	}

	int Destroy() {
		auto prevTransEHtoCEH = _set_se_translator(PythonTransSEHtoCEH);
		auto ret = _Destroy();
		_set_se_translator(prevTransEHtoCEH);
		return ret;
	}
}

