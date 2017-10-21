#include "exception_translator.h"
#include "output_debugstream.h"

#include <windows.h>
#include <string>



std::wstring python_critical_exception_message() {
	auto err = L""
		"hmPython3で致命的エラーが発生しました。\n"
		"\n"
		"・ライブラリの不正な多重解放\n"
		"・複数回の初期化に対応していないパッケージの利用(numpy/pythonnet等)\n"
		"\n"
		"が疑われます。\n\n"
		"「不正なメモリ状態」となったため、プロセスは正常に継続使用できません。\n"
		"秀丸上の必要なファイルを保存し、「この秀丸プロセス」を終了してください。\n";
	return err;
}




// 例外発生時に関数の呼び出し履歴を表示する、例外フィルタ関数 */
/*
#include "dbgHelp.h"

#pragma comment(lib, "dbghelp.lib")

LONG CALLBACK SWFilter(EXCEPTION_POINTERS *ExInfo)
{
	STACKFRAME sf;
	BOOL bResult;
	PIMAGEHLP_SYMBOL pSym;
	DWORD Disp;

	// シンボル情報格納用バッファの初期化
	pSym = (PIMAGEHLP_SYMBOL)GlobalAlloc(GMEM_FIXED, 10000);
	pSym->SizeOfStruct = 10000;
	pSym->MaxNameLength = 10000 - sizeof(IMAGEHLP_SYMBOL);

	// スタックフレームの初期化/
	ZeroMemory(&sf, sizeof(sf));
	sf.AddrPC.Offset = ExInfo->ContextRecord->Eip;
	sf.AddrStack.Offset = ExInfo->ContextRecord->Esp;
	sf.AddrFrame.Offset = ExInfo->ContextRecord->Ebp;
	sf.AddrPC.Mode = AddrModeFlat;
	sf.AddrStack.Mode = AddrModeFlat;
	sf.AddrFrame.Mode = AddrModeFlat;

	// シンボルハンドラの初期化
	SymInitialize(GetCurrentProcess(), NULL, TRUE);

	wstring errmsg = L"例外発生:\n";

	wchar_t buf[1024];

	// スタックフレームを順に表示していく
	for (;;) {
		// 次のスタックフレームの取得
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

		// 失敗ならば、ループを抜ける
		if (!bResult || sf.AddrFrame.Offset == 0) {
			break;
		}

		// プログラムカウンタから関数名を取得
		bResult = SymGetSymFromAddr(GetCurrentProcess(), sf.AddrPC.Offset, &Disp, pSym);



		/./ 取得結果を表示
		if (bResult) {
			wsprintf(buf, L"0x%08x %s() + 0x%x\n", sf.AddrPC.Offset, pSym->Name, Disp);
		}
		else {
			wsprintf(buf, L"%08x, ---\n", sf.AddrPC.Offset);
		}

		errmsg += buf;
	}

	// 後処理
	SymCleanup(GetCurrentProcess());
	GlobalFree(pSym);

	MessageBox(NULL, errmsg.data(), L"システム例外", NULL);

	return(EXCEPTION_EXECUTE_HANDLER);
}

*/