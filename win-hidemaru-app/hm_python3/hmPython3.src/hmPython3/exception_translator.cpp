#include "exception_translator.h"
#include "output_debugstream.h"

#include <string>

std::wstring python_critical_exception_message() {
	auto err = L"hmPython3�ɂ����āA\n"
		"�u���C�u�����̕s���ȉ���v���邢�́u�����������Ȃ��p�b�P�[�W���p�v���A\n"
		"�v���I�ȑ��삪�s��ꂽ�悤�ł��B\n\n"
		"hmPython3�͂��̃v���Z�X���ł͋@�\�𐳂����p���ł��܂���B\n\n"
		"���X�ɏG�ۏ�̕K�v�ȃt�@�C����ۑ����A�u���̏G�ۃv���Z�X�v���I�����邱�Ƃ����������߂��܂��B\n";
	return err;
}