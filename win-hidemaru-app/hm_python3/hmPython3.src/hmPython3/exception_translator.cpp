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




