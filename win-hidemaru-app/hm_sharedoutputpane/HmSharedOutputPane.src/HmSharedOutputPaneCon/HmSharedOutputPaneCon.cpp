// HmSharedOutputPaneCon.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <windows.h>
#include <string>

using namespace std;

const wstring strDLLFullPath = L"HmSharedOutputPane.dll";

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
