#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <shlwapi.h>
#include <string>
#include "assemblerjumper.h"
#pragma comment(lib, "shlwapi.lib")

#include "outputdebugstream.h"
#include "autocompletewindow.h"
#include "hookwin32api.h"
#include "outlinerwindow.h"
#include "programmemory.h"
#include "assemblerjumper.h"

#include "OnigRegex.h"


using namespace std;

extern HANDLE curProcess;

HWND hWndHidemaruOutLinerListBox = NULL;
static BOOL _stdcall EnumChildProc(HWND wnd, LPARAM lParam) {
	char cname[250];
	GetClassName(wnd, cname, sizeof(cname));

	char stext[250];
	GetWindowText(wnd, stext, sizeof(stext));

	OutputDebugStream("ウインドウ%x, クラス名:%s テキスト%s:\n", wnd, cname, stext);
	if (strcmp(cname, "ListBox") == 0) {
		OutputDebugStream("リストボックス発見%x, クラス名:%s\n", wnd, cname);

		HWND parent = GetParent(wnd);
		GetClassName(parent, cname, sizeof(cname));
		OutputDebugStream("リストボックスの親%x, クラス名:%s\n", parent, cname);

		// 親がアウトライナで、自分はリストボックス。確実にアウトライナ用の分析枠
		if (strcmp(cname, "HM32OUTLINE") == 0) {
			hWndHidemaruOutLinerListBox = wnd;
		}
	}


	return TRUE;
}

/*
*/


static int iEAXOfHidemaruTitleAdd = 0;

void OnCallBackedOfHidemaruTitleAdd() {
	OutputDebugStream("■タイトル更新");
}

int iCallFuncOfHidemaruTitleAdd = 0;

static int pJumpFromOfHidemaruTitleAdd = NULL;
static int pRetnBackOfHidemaruTitleAdd = NULL;
static int iPuashBackValueOfHidemaruTitleAdd = 0;

extern "C" __declspec(dllexport) __declspec(naked) void JumpOnCallBackedOfHidemaruTitleAdd() {

	__asm {
		mov iEAXOfHidemaruTitleAdd, eax			// タイトル
		push iPuashBackValueOfHidemaruTitleAdd	// 「 - 秀丸」っていう文字のプッシュ(英語だと「 - Maruo」

		PUSHAD
		PUSHFD
	}

	OnCallBackedOfHidemaruTitleAdd();

	__asm {

		POPFD
		POPAD

		jmp pRetnBackOfHidemaruTitleAdd
	}
}









extern void HookWin32apiFuncs();
extern int ProgramMemoryDstImageBase;
extern int ProgramMemoryDstBaseOfData;
extern char szHidemaru[MAX_PATH*2];

char szCurrentFileOnCallSetHidemaruHandle[MAX_PATH * 2] = ""; // 秀丸のSetHidemaruHandleが呼ばれた時に最初にスロットに入るファイル名。途中でファイル名が変わることがあるので、あくまでもなかった場合用途
extern HWND hWndHidemaru;
#define WM_HIDEMARUINFO (WM_USER + 181)

#define HIDEMARUINFO_GETTABWIDTH 0
#define HIDEMARUINFO_GETSTARTCOLUMN 1
#define HIDEMARUINFO_GETSPACETAB 2
#define HIDEMARUINFO_GETMANUALTAB 3
#define HIDEMARUINFO_GETFILEFULLPATH 4

char *GetCurrentFileName() {
	if (hWndHidemaru) {
		// 現在の秀丸ウィンドウのファイル名を得る。
		LRESULT cwch = SendMessage(hWndHidemaru, WM_HIDEMARUINFO, HIDEMARUINFO_GETFILEFULLPATH, (LPARAM)szCurrentFileOnCallSetHidemaruHandle);
		return szCurrentFileOnCallSetHidemaruHandle;
	}
	else {
		return "";
	}
}

char szHidemaruAdditionTitle[] = " - 秀丸";

byte pRefOfByteArrayOfHidemaruTitileAdd[] = { 0x68, 0x00, 0x00, 0x00, 0x00, 0x50, 0xE8 };

HWND hWndHidemaru = NULL;

extern "C" __declspec(dllexport) int SetHidemaruHandle(HWND pWndHidemaru, char *szCurFileName) {
	OutputDebugStream("■変わった瞬間のファイル名%s\n", szCurFileName);
	strcpy(szCurrentFileOnCallSetHidemaruHandle, szCurFileName);
	HookWin32apiFuncs();

	hWndHidemaru = pWndHidemaru = GetForegroundWindow();
	char sz[100];
	GetClassName((HWND)pWndHidemaru, sz, sizeof(sz));
	OutputDebugStream("渡されたウィンドウ%x, 名前%s\n\n", pWndHidemaru, sz);

	HWND wnd = GetForegroundWindow();
	GetClassName((HWND)wnd, sz, sizeof(sz));
	OutputDebugStream("フォアなウィンドウ%x:名前%s\n", wnd, sz);

	if (wnd) {
		EnumChildWindows((HWND)wnd, EnumChildProc, 0);
		// ★★★
		AnalizeOutlinerListBox(hWndHidemaruOutLinerListBox);
	}

	if (iCallFuncOfHidemaruTitleAdd == 0) {
		checkProgramMemoryImage(szHidemaru);

		OutputDebugStream("ベースアドレス%x\n", ProgramMemoryDstImageBase);
		OutputDebugStream("データアドレス%x\n", ProgramMemoryDstBaseOfData);

		// " - 秀丸" を探す
		void *pHidemaruTitileAddInBaseOfData = memmem((void *)ProgramMemoryDstBaseOfData, iProgramMemorySearchSize, szHidemaruAdditionTitle, sizeof(szHidemaruAdditionTitle));
		if (pHidemaruTitileAddInBaseOfData == NULL) {
			pHidemaruTitileAddInBaseOfData = memmem((void *)ProgramMemoryDstBaseOfData, iProgramMemorySearchSize, " - Maruo", sizeof(szHidemaruAdditionTitle)); // 英語版
		}
		OutputDebugStream("pHidemaruTitileAddInBaseOfDataの位置%x\n", pHidemaruTitileAddInBaseOfData);
		iPuashBackValueOfHidemaruTitleAdd = (int)pHidemaruTitileAddInBaseOfData;

		// サーチするための内容を決定する
		byte *p = pRefOfByteArrayOfHidemaruTitileAdd;
		p++;
		int *ip = (int *)p;
		(*ip) = (int)pHidemaruTitileAddInBaseOfData;

		/*
		OutputDebugStream("%x", pRefOfByteArrayOfHidemaruTitileAdd[0]);
		OutputDebugStream("%x", pRefOfByteArrayOfHidemaruTitileAdd[1]);
		OutputDebugStream("%x", pRefOfByteArrayOfHidemaruTitileAdd[2]);
		OutputDebugStream("%x", pRefOfByteArrayOfHidemaruTitileAdd[3]);
		OutputDebugStream("%x", pRefOfByteArrayOfHidemaruTitileAdd[4]);
		OutputDebugStream("%x", pRefOfByteArrayOfHidemaruTitileAdd[5]);
		OutputDebugStream("%x\n", pRefOfByteArrayOfHidemaruTitileAdd[6]);
		*/

		void *pPushAsmOfHidemaruTitileAdd = memmem((void *)ProgramMemoryDstImageBase, (ProgramMemoryDstBaseOfData - ProgramMemoryDstImageBase), pRefOfByteArrayOfHidemaruTitileAdd, sizeof(pRefOfByteArrayOfHidemaruTitileAdd));
		if (pPushAsmOfHidemaruTitileAdd == NULL) {
			void *pPushAsmOfHidemaruTitileAdd = memmem((void *)ProgramMemoryDstImageBase, (ProgramMemoryDstBaseOfData - ProgramMemoryDstImageBase), pRefOfByteArrayOfHidemaruTitileAdd, sizeof(pRefOfByteArrayOfHidemaruTitileAdd));
		}
		OutputDebugStream("秀のタイトルのpushのプログラムの場所%x\n", pPushAsmOfHidemaruTitileAdd);
		iCallFuncOfHidemaruTitleAdd = (int)pPushAsmOfHidemaruTitileAdd;
	}

	char cmdOnTenshouExeJumpFrom[10] = "\xE9\x90\x90\x90\x90\x90\x90\x90\x90"; // 多めにとっておく

	pJumpFromOfHidemaruTitleAdd = iCallFuncOfHidemaruTitleAdd;
	pRetnBackOfHidemaruTitleAdd = pJumpFromOfHidemaruTitleAdd + 5;

	// まずアドレスを数字として扱う
	int iAddress = (int)JumpOnCallBackedOfHidemaruTitleAdd;
	int SubAddress = iAddress - (pJumpFromOfHidemaruTitleAdd + 5);

	// ５というのは、0046C194  -E9 ????????  JMP TSMod.OnTSExeCreateFileA01  の命令に必要なバイト数。要するに５バイト足すと次のニーモニック命令群に移動するのだ。そしてそこからの差分がジャンプする際の目的格として利用される。
	memcpy(cmdOnTenshouExeJumpFrom + 1, &SubAddress, 4); // +1 はE9の次から4バイト分書き換えるから。

	/*
	OutputDebugStream("ジャンプ先%x", JumpOnCallBackedOfHidemaruTitleAdd);
	OutputDebugStream("%x,", cmdOnTenshouExeJumpFrom[0]);
	OutputDebugStream("%x,", cmdOnTenshouExeJumpFrom[1]);
	OutputDebugStream("%x,", cmdOnTenshouExeJumpFrom[2]);
	OutputDebugStream("%x,", cmdOnTenshouExeJumpFrom[3]);
	OutputDebugStream("%x,", cmdOnTenshouExeJumpFrom[4]);
	OutputDebugStream("%x\n", 0);
	*/

	// 構築したニーモニック命令をTENSHOU.EXEのメモリに書き換える
	OutputDebugStream("書き込む%x\n", pJumpFromOfHidemaruTitleAdd);
	WriteProcessMemory(curProcess, (LPVOID)(pJumpFromOfHidemaruTitleAdd), cmdOnTenshouExeJumpFrom, 5, NULL); //5バイトのみ書き込む

	return TRUE;
}

