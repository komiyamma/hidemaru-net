

#include "exception_translator.h"
#include "output_debugstream.h"

#include <windows.h>
#include <string>
#include "convert_string.h"

#include "imagehlp.h"
#pragma comment(lib, "imagehlp.lib")

wstring CSEHException::what() {

	wstring ret =L"hmPython3で致命的エラーが発生しました。\n\n";

	PEXCEPTION_RECORD exRecord = m_ExceptionPointers->ExceptionRecord;

	printf("|Exception Code:    0x%08x\n", exRecord->ExceptionCode);
	printf("|Exception Falgs:   0x%08x\n", exRecord->ExceptionFlags);
	printf("|Exception Address: 0x%p\n", exRecord->ExceptionAddress);
	printf("|Parameters:\n");

	STACKFRAME sf;

	// スタックフレームの初期化/
	ZeroMemory(&sf, sizeof(sf));
	auto ctRecord = m_ExceptionPointers->ContextRecord;
	sf.AddrPC.Offset = ctRecord->Eip;
	sf.AddrStack.Offset = ctRecord->Esp;
	sf.AddrFrame.Offset = ctRecord->Ebp;
	sf.AddrPC.Mode = AddrModeFlat;
	sf.AddrStack.Mode = AddrModeFlat;
	sf.AddrFrame.Mode = AddrModeFlat;

	auto hProcess = GetCurrentProcess();

	// シンボルハンドラの初期化
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

	// 後処理
	SymCleanup(GetCurrentProcess());
	GlobalFree(pSym);

	ret += L""
		"\n"
		"・ライブラリの不正な多重解放\n"
		"・複数回の初期化に対応していないパッケージの利用(numpy/pythonnet等)\n"
		"\n"
		"が疑われます。\n\n"
		"「不正なメモリ状態」となったため、プロセスは正常に継続できません。\n"
		"「この秀丸プロセス」は、おそらくこのまま強制終了します。\n";

	return ret;
}



void PythonTransSEHtoCEH(unsigned int ExceptionCode, PEXCEPTION_POINTERS ExceptionPointers) {
	throw CSEHException(ExceptionCode, ExceptionPointers);
}



void PythonAlreadySetException(py::error_already_set &e) {
	OutputDebugStream(L"エラー:\n" + utf8_to_utf16(e.what()));
}
void PythonKnknownException(exception &e) {
	OutputDebugStream(L"エラー:\n" + utf8_to_utf16(e.what()));
}
void SystemAlreadySetException(CSEHException &e) {
	auto err = e.what();
	OutputDebugStream((L"エラー:\n" + err).data());
	MessageBox(NULL, err.data(), L"システム例外", NULL);
}
void SystemUnknownException() {
	OutputDebugStream(L"エラー:\nシステム例外(...)");
	MessageBox(NULL, L"捕捉出来ないシステム例外が発生しました。", L"システム例外", NULL);
}
