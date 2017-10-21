#include "exception_translator.h"
#include "output_debugstream.h"

#include <string>

std::wstring python_critical_exception_message() {
	auto err = L"hmPython3において、\n"
		"「ライブラリの不正な解放」あるいは「正しく扱えないパッケージ利用」等、\n"
		"致命的な操作が行われたようです。\n\n"
		"hmPython3はこのプロセス内では機能を正しく継続できません。\n\n"
		"早々に秀丸上の必要なファイルを保存し、「この秀丸プロセス」を終了することを強くお勧めします。\n";
	return err;
}