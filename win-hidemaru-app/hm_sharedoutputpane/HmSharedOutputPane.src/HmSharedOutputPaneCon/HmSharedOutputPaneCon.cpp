// HmSharedOutputPaneCon.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <windows.h>
#include <string>

using namespace std;

// 秀丸のディレクトリの指定。この場合は、「C:\usr\hidemaru\hidemaru.exe」という位置関係にある。
const wstring strDLLFullPath = LR"(C:\usr\hidemaru\HmSharedOutputPane.dll)";

using PFNSETSHAREDMESSAGEW = void(_stdcall*)(const wchar_t* wszmsg);

HMODULE dll = NULL;
int wmain(int argc, wchar_t *argv[]) {
	if (argc <= 1) {
		return 0;
	}
	dll = LoadLibraryW(strDLLFullPath.data());
	if (dll) {
		PFNSETSHAREDMESSAGEW hmOutPut = (PFNSETSHAREDMESSAGEW)GetProcAddress(dll, "SetSharedMessageW");
		if (hmOutPut) {
			hmOutPut(argv[1]);
		}
	}
}
