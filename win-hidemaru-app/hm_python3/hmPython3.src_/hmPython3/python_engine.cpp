
#include "python_hidemaru_lib.h"
#include "python_engine.h"

namespace PythonEngine {

	// �G���W�����L���ɂȂ���
	BOOL m_isValid = false;
	// �G�ۂ̖��O�i�[
	wchar_t *m_wstr_program = NULL;

	wchar_t szHidemaruFullPath[MAX_PATH] = L"";

	BOOL IsValid() {
		return m_isValid;
	}

	// �G���W������
	int Create()
	{
		m_isValid = FALSE;

		// XML�̐ݒ�ɏ]����Python�̃p�X���擾
		wstring python_home = GetPythonPath();
		if (python_home.size() == 0) {
			return FALSE;
		}

		try {
			// �p�C�\���̃z�[���ݒ�
			Py_SetPythonHome((wchar_t *)python_home.data());

			// �r���g�C���Ƃ��Ē�`����
			PyImport_AppendInittab("hidemaru", PyInit_hidemaru);

			GetModuleFileName(NULL, szHidemaruFullPath, _countof(szHidemaruFullPath));

			// �����̖��O�̓`�B�B(���΃p�X�̉��߂̂��߂ɗ��p�����Ǝv����)
			m_wstr_program = szHidemaruFullPath;
			Py_SetProgramName(m_wstr_program);

			// ���܂蕶��
			py::initialize_interpreter();
			{
				//	py::module::import("hidemaru");
				// �L���b�V�������Ȃ����߂�sys�̃C���|�[�g�B
				py::eval<py::eval_single_statement>("import sys");
				// �G�ۗp�̃N���X�̃C���|�[�g�B���ŁA����o�R�ŏ����������ꍇ�Ȃǂɗ��p����
				py::eval<py::eval_single_statement>("import hidemaru");
				// python���C�u�����Ȃǂ��g�p�������ɃL���b�V�����I舂ɍ��Ȃ��悤�ɁB(�����łȂ��Ƃ��������ɃL���b�V�����o����)
				py::eval<py::eval_single_statement>("sys.dont_write_bytecode = True");

				// ���C�������ǂݍ���
				auto global = py::dict(py::module::import("__main__").attr("__dict__"));
			}
#pragma region
			/*
			// DestroyScope�̒�`
			py::eval<py::eval_statements>(
				"def DestroyScope():\n"
				"    pass");
			*/
#pragma endregion
			// �����܂Ŏ��s�o������A�G���W���Ƃ��ėL���ɂȂ����Ƃ����t���O������
			m_isValid = TRUE;
			return TRUE;
		}
		catch (py::error_already_set& e) {
			OutputDebugStream(L"�G���[:\n" + utf8_to_utf16(e.what()));
		}

		// �G���W���Ƃ��đʖ�
		return FALSE;
	}

	// �G���W�����\�z���ꂽ��ɂP�񂾂����s����悤��
	static bool m_isInitialize = FALSE;
	int Initialize() {
		if (!IsValid()) {
			return FALSE;
		}
		try {
			if (!m_isInitialize) {
				auto global = py::dict(py::module::import("__main__").attr("__dict__"));
				auto local = py::dict();
				// �}�N�����Ăяo�������̃t�H���_��python�t�@�C���̒u����Ƃ��Ă��F������
				py::eval<py::eval_single_statement>("sys.path.append( hidemaru.macro.get_var('currentmacrodirectory') )");

				m_isInitialize = TRUE;
			}
		}
		catch (py::error_already_set& e) {
			OutputDebugStream(L"�G���[:\n" + utf8_to_utf16(e.what()));
		}

		return TRUE;
	}

	// �Ώۂ̃V���{�����̒l�𐔒l�Ƃ��ē���
	intHM_t GetNumVar(wstring utf16_simbol) {
		try {
			auto global = py::dict(py::module::import("__main__").attr("__dict__"));
			auto local = py::dict();

			// �l�𓾂āc
			string utf8_simbol = utf16_to_utf8(utf16_simbol);
			auto value = global[utf8_simbol.c_str()];

			// �����񉻂���̂́A���Ԃ��Ȃ�̃I�u�W�F�N�g���킩��Ȃ�����
			string utf8_value = py::str(value);
			wstring utf16_value = utf8_to_utf16(utf8_value);

			// �����𐔒l�ɕϊ��g���C�B�_���Ȃ�0����B
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
			OutputDebugStream(L"�G���[:\n" + utf8_to_utf16(e.what()));
		}

		return 0;
	}

	// �Ώۂ̃V���{�����̒l�ɐ��l��������
	BOOL SetNumVar(wstring utf16_simbol, intHM_t value) {
		try {
			auto global = py::dict(py::module::import("__main__").attr("__dict__"));
			auto local = py::dict();

			// �l����
			string utf8_simbol = utf16_to_utf8(utf16_simbol);
			global[utf8_simbol.c_str()] = value;

			return TRUE;
		}
		catch (py::error_already_set& e) {
			OutputDebugStream(L"�G���[:\n" + utf8_to_utf16(e.what()));
		}

		return FALSE;
	}

	// �Ώۂ̃V���{�����̒l�𕶎���Ƃ��ē���
	wstring GetStrVar(wstring utf16_simbol) {
		try {
			auto global = py::dict(py::module::import("__main__").attr("__dict__"));
			auto local = py::dict();

			// �l�𓾂āc
			string utf8_simbol = utf16_to_utf8(utf16_simbol);
			auto value = global[utf8_simbol.c_str()];

			// �����񉻂���
			string utf8_value = py::str(value);

			wstring utf16_value = utf8_to_utf16(utf8_value);
			return utf16_value;
		}
		catch (py::error_already_set& e) {
			OutputDebugStream(L"�G���[:\n" + utf8_to_utf16(e.what()));
		}

		return L"";
	}

	// �Ώۂ̃V���{�����̒l�ɕ������������
	BOOL SetStrVar(wstring utf16_simbol, wstring utf16_value) {
		try {
			auto global = py::dict(py::module::import("__main__").attr("__dict__"));
			auto local = py::dict();

			// �l����
			string utf8_simbol = utf16_to_utf8(utf16_simbol);
			string utf8_value = utf16_to_utf8(utf16_value);
			global[utf8_simbol.c_str()] = utf8_value;

			return TRUE;
		}
		catch (py::error_already_set& e) {
			OutputDebugStream(L"�G���[:\n" + utf8_to_utf16(e.what()));
		}

		return FALSE;
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
		py::eval<py::eval_single_statement>(utf16_to_utf8(L"print('������')"));
		// Python �Ŋ֐����쐬 (Name=Hoge, Job=Teacher)
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

	// �Ώۂ̕������Python�̕������Ƃ݂Ȃ��ĕ]������
	int DoString(wstring utf16_expression) {
		if (!IsValid()) {
			return FALSE;
		}

		try {

			string utf8_expression = utf16_to_utf8(utf16_expression);
			py::eval<py::eval_statements>(utf8_expression);

			return TRUE;
		}
		catch (py::error_already_set& e) {
			OutputDebugStream(L"�G���[:\n" + utf8_to_utf16(e.what()));
		}

		return FALSE;
	}

	// �G���W���̔j��
	int Destroy() {

		// �L���łȂ��Ȃ�΁A���I��
		if (!IsValid()) {
			return FALSE;
		}

		// �L���Ȏ�����
		if (m_isValid) {

			try {
				// DestroyScope�Ƃ����̂��A���C�����W���[�����ɒ�`����Ă���΁A��������s����
				auto global = py::dict(py::module::import("__main__").attr("__dict__"));

				// �}�N�����Ăяo�������̃t�H���_��python�t�@�C���̒u����Ƃ��Ă��F������
				py::eval<py::eval_statements>("if 'DestroyScope' in globals(): DestroyScope()");
#pragma region
				/*
				auto func = global["DestroyScope"];
				func();
				*/
#pragma endregion
			}
			catch (py::error_already_set& e) {
				OutputDebugStream(L"�G���[:\n" + utf8_to_utf16(e.what()));
			}

			// �j��
			try {
				py::finalize_interpreter();

				// PyMem_RawFree(m_wstr_program);
			}
			catch (py::error_already_set& e) {
				OutputDebugStream(L"�G���[:\n" + utf8_to_utf16(e.what()));
			}
		}

		// ������Ԃ�
		m_isValid = FALSE;
		m_isInitialize = FALSE;
		// m_wstr_program = NULL;

		return TRUE;
	}

}

