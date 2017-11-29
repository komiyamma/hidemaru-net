#include "exception_translator.h"
#include "output_debugstream.h"

#include <windows.h>
#include <string>



std::wstring python_critical_exception_message() {
	auto err = L""
		"hmPython3�Œv���I�G���[���������܂����B\n"
		"\n"
		"�E���C�u�����̕s���ȑ��d���\n"
		"�E������̏������ɑΉ����Ă��Ȃ��p�b�P�[�W�̗��p(numpy/pythonnet��)\n"
		"\n"
		"���^���܂��B\n\n"
		"�u�s���ȃ�������ԁv�ƂȂ������߁A�v���Z�X�͐���Ɍp���g�p�ł��܂���B\n"
		"�G�ۏ�̕K�v�ȃt�@�C����ۑ����A�u���̏G�ۃv���Z�X�v���I�����Ă��������B\n";
	return err;
}




