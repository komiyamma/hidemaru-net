#include "exception_translator.h"
#include "output_debugstream.h"

#include <windows.h>
#include <string>



std::wstring python_critical_exception_message() {
	auto err = L"hmPython3�ɂ����āA\n"
		"�u���C�u�����̕s���ȉ���v���邢�́u�����������Ȃ��p�b�P�[�W���p�v���A\n"
		"�v���I�ȑ��삪�s��ꂽ�悤�ł��B\n\n"
		"hmPython3�͂��̃v���Z�X���ł͋@�\�𐳂����p���ł��܂���B\n\n"
		"���X�ɏG�ۏ�̕K�v�ȃt�@�C����ۑ����A�u���̏G�ۃv���Z�X�v���I�����邱�Ƃ����������߂��܂��B\n";
	return err;
}




// ��O�������Ɋ֐��̌Ăяo��������\������A��O�t�B���^�֐� */
/*
#include "dbgHelp.h"

#pragma comment(lib, "dbghelp.lib")

LONG CALLBACK SWFilter(EXCEPTION_POINTERS *ExInfo)
{
	STACKFRAME sf;
	BOOL bResult;
	PIMAGEHLP_SYMBOL pSym;
	DWORD Disp;

	// �V���{�����i�[�p�o�b�t�@�̏�����
	pSym = (PIMAGEHLP_SYMBOL)GlobalAlloc(GMEM_FIXED, 10000);
	pSym->SizeOfStruct = 10000;
	pSym->MaxNameLength = 10000 - sizeof(IMAGEHLP_SYMBOL);

	// �X�^�b�N�t���[���̏�����/
	ZeroMemory(&sf, sizeof(sf));
	sf.AddrPC.Offset = ExInfo->ContextRecord->Eip;
	sf.AddrStack.Offset = ExInfo->ContextRecord->Esp;
	sf.AddrFrame.Offset = ExInfo->ContextRecord->Ebp;
	sf.AddrPC.Mode = AddrModeFlat;
	sf.AddrStack.Mode = AddrModeFlat;
	sf.AddrFrame.Mode = AddrModeFlat;

	// �V���{���n���h���̏�����
	SymInitialize(GetCurrentProcess(), NULL, TRUE);

	wstring errmsg = L"��O����:\n";

	wchar_t buf[1024];

	// �X�^�b�N�t���[�������ɕ\�����Ă���
	for (;;) {
		// ���̃X�^�b�N�t���[���̎擾
		bResult = StackWalk(
			IMAGE_FILE_MACHINE_I386,
			GetCurrentProcess(),
			GetCurrentThread(),
			&sf,
			NULL,
			NULL,
			SymFunctionTableAccess,
			SymGetModuleBase,
			NULL);

		// ���s�Ȃ�΁A���[�v�𔲂���
		if (!bResult || sf.AddrFrame.Offset == 0) {
			break;
		}

		// �v���O�����J�E���^����֐������擾
		bResult = SymGetSymFromAddr(GetCurrentProcess(), sf.AddrPC.Offset, &Disp, pSym);



		/./ �擾���ʂ�\��
		if (bResult) {
			wsprintf(buf, L"0x%08x %s() + 0x%x\n", sf.AddrPC.Offset, pSym->Name, Disp);
		}
		else {
			wsprintf(buf, L"%08x, ---\n", sf.AddrPC.Offset);
		}

		errmsg += buf;
	}

	// �㏈��
	SymCleanup(GetCurrentProcess());
	GlobalFree(pSym);

	MessageBox(NULL, errmsg.data(), L"�V�X�e����O", NULL);

	return(EXCEPTION_EXECUTE_HANDLER);
}

*/