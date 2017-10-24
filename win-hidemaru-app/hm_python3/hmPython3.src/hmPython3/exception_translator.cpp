

#include "exception_translator.h"
#include "output_debugstream.h"

#include <windows.h>
#include <string>
#include "convert_string.h"

#include "imagehlp.h"
#pragma comment(lib, "imagehlp.lib")

wstring CSEHException::what() {

	wstring ret =L"hmPython3�Œv���I�G���[���������܂����B\n\n";

	PEXCEPTION_RECORD exRecord = m_ExceptionPointers->ExceptionRecord;

	printf("|Exception Code:    0x%08x\n", exRecord->ExceptionCode);
	printf("|Exception Falgs:   0x%08x\n", exRecord->ExceptionFlags);
	printf("|Exception Address: 0x%p\n", exRecord->ExceptionAddress);
	printf("|Parameters:\n");

	STACKFRAME sf;

	// �X�^�b�N�t���[���̏�����/
	ZeroMemory(&sf, sizeof(sf));
	auto ctRecord = m_ExceptionPointers->ContextRecord;
	sf.AddrPC.Offset = ctRecord->Eip;
	sf.AddrStack.Offset = ctRecord->Esp;
	sf.AddrFrame.Offset = ctRecord->Ebp;
	sf.AddrPC.Mode = AddrModeFlat;
	sf.AddrStack.Mode = AddrModeFlat;
	sf.AddrFrame.Mode = AddrModeFlat;

	auto hProcess = GetCurrentProcess();

	// �V���{���n���h���̏�����
	SymInitialize(hProcess, NULL, TRUE);

	wchar_t buf[4096];
	IMAGEHLP_MODULE imageModule = { sizeof(IMAGEHLP_MODULE) };

	BOOL bResult;
	PIMAGEHLP_SYMBOL pSym;
	DWORD Disp;

	bResult = SymGetModuleInfo(hProcess, sf.AddrPC.Offset, &imageModule);
	if (bResult) {
		wsprintf(buf, L"%s\n", cp932_to_utf16(imageModule.ImageName).data());
		ret += buf;
	}
	else {
		wsprintf(buf, L"%08x, ---\n", sf.AddrPC.Offset);
	}

	bResult = SymGetSymFromAddr(hProcess, sf.AddrPC.Offset, &Disp, pSym);
	if (bResult) {
		wsprintf(buf, L"0x%08x %s() + 0x%x\n", sf.AddrPC.Offset, cp932_to_utf16(pSym->Name).data(), Disp);
		ret += buf;
	}
	else {
		wsprintf(buf, L"%08x, ---\n", sf.AddrPC.Offset);
	}

	// �㏈��
	SymCleanup(GetCurrentProcess());
	GlobalFree(pSym);

	ret += L""
		"\n"
		"�E���C�u�����̕s���ȑ��d���\n"
		"�E������̏������ɑΉ����Ă��Ȃ��p�b�P�[�W�̗��p(numpy/pythonnet��)\n"
		"\n"
		"���^���܂��B\n\n"
		"�u�s���ȃ�������ԁv�ƂȂ������߁A�v���Z�X�͐���Ɍp���ł��܂���B\n"
		"�u���̏G�ۃv���Z�X�v�́A�����炭���̂܂܋����I�����܂��B\n";

	return ret;
}



void PythonTransSEHtoCEH(unsigned int ExceptionCode, PEXCEPTION_POINTERS ExceptionPointers) {
	throw CSEHException(ExceptionCode, ExceptionPointers);
}



void PythonAlreadySetException(py::error_already_set &e) {
	OutputDebugStream(L"�G���[:\n" + utf8_to_utf16(e.what()));
}
void PythonKnknownException(exception &e) {
	OutputDebugStream(L"�G���[:\n" + utf8_to_utf16(e.what()));
}
void SystemAlreadySetException(CSEHException &e) {
	auto err = e.what();
	OutputDebugStream((L"�G���[:\n" + err).data());
	MessageBox(NULL, err.data(), L"�V�X�e����O", NULL);
}
void SystemUnknownException() {
	OutputDebugStream(L"�G���[:\n�V�X�e����O(...)");
	MessageBox(NULL, L"�ߑ��o���Ȃ��V�X�e����O���������܂����B", L"�V�X�e����O", NULL);
}
