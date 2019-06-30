#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <tlhelp32.h>
#include <dbghelp.h>

#include <vector>
using namespace std;

// ImageDirectoryEntryToData
#pragma comment(lib, "dbghelp.lib")


// ひとつのモジュールに対してAPIフックを行う関数
void ReplaceIATEntryInOneMod(
	PCSTR pszModuleName,
	PROC pfnCurrent,
	PROC pfnNew,
	HMODULE hmodCaller)
{
	ULONG ulSize;
	PIMAGE_IMPORT_DESCRIPTOR pImportDesc;
	pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData(
		hmodCaller, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &ulSize);

	if (pImportDesc == NULL)
		return;

	while (pImportDesc->Name) {
		PSTR pszModName = (PSTR)((PBYTE)hmodCaller + pImportDesc->Name);
		if (lstrcmpiA(pszModName, pszModuleName) == 0)
			break;
		pImportDesc++;
	}

	if (pImportDesc->Name == 0)
		return;

	PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)
		((PBYTE)hmodCaller + pImportDesc->FirstThunk);

	while (pThunk->u1.Function) {
		PROC *ppfn = (PROC*)&pThunk->u1.Function;
		BOOL fFound = (*ppfn == pfnCurrent);
		if (fFound) {
			DWORD dwDummy;
			VirtualProtect(ppfn, sizeof(ppfn), PAGE_EXECUTE_READWRITE, &dwDummy);
			WriteProcessMemory(GetCurrentProcess(), ppfn, &pfnNew, sizeof(pfnNew), NULL);
			return;
		}
		pThunk++;
	}
	return;
}

// すべてのモジュールに対してAPIフックを行う関数
void ReplaceIATEntryInAllMods(
	PCSTR pszModuleName,
	PROC pfnCurrent,
	PROC pfnNew)
{
	// モジュールリストを取得
	HANDLE hModuleSnap = CreateToolhelp32Snapshot(
		TH32CS_SNAPMODULE, GetCurrentProcessId());
	if (hModuleSnap == INVALID_HANDLE_VALUE)
		return;

	MODULEENTRY32 me;
	me.dwSize = sizeof(me);
	BOOL bModuleResult = Module32First(hModuleSnap, &me);
	// それぞれのモジュールに対してReplaceIATEntryInOneModを実行
	while (bModuleResult) {
		ReplaceIATEntryInOneMod(pszModuleName, pfnCurrent, pfnNew, me.hModule);
		bModuleResult = Module32Next(hModuleSnap, &me);
	}
	CloseHandle(hModuleSnap);
}

struct HOOKOBJECT {
	string strModuleName;
	string strFuncName;
	void *pFuncAddress;

	BOOL operator==(HOOKOBJECT o) {
		if (this->strModuleName == o.strModuleName &&
			this->strFuncName == o.strFuncName &&
			this->pFuncAddress == o.pFuncAddress) {
			return TRUE;
		}
		return FALSE;
	}
};

vector<HOOKOBJECT> hookmap;


extern "C" /*__declspec(dllexport)*/ PROC hook_win32api(char *szModuleName, char *szFuncName, void *szFuncAddress) {
	PROC pfnOrig;
	HOOKOBJECT ho;
	ho.strModuleName = string(szModuleName);
	ho.strFuncName = string(szFuncName);
	ho.pFuncAddress = szFuncAddress;

	auto it = std::find(hookmap.begin(), hookmap.end(), ho);
	if (it == hookmap.end()) {
		hookmap.push_back(ho);

		pfnOrig = ::GetProcAddress(GetModuleHandleA(szModuleName), szFuncName);
		ReplaceIATEntryInAllMods(szModuleName, pfnOrig, (PROC)szFuncAddress);
		return pfnOrig;
	}
	return NULL;
}





