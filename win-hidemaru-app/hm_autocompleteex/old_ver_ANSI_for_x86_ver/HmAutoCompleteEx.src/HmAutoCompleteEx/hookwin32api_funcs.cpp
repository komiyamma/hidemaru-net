#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <vector>
#include <map>
#include <process.h>
#include "autocompletewindow.h"
#include "outputdebugstream.h"
#include "hookwin32api.h"
#include "autocomletehelptips.h"

#include "OnigRegex.h"

using namespace std;


//-----------------------------------------------------------------------------------------------------------------------------------
// SendMessageA / SendMessageW
//-------------------------------------------------------------------------------------------------------------------------------------
extern "C" /*__declspec(dllexport)*/ PROC hook(char *szModuleName, char *szFuncName, void *szFuncAddress);


// フックする関数のプロトタイプを定義
using PFNSENDMESSAGEA = LRESULT(WINAPI *)(HWND, UINT, WPARAM, LPARAM);

PROC pfnPrevSendMessageA = NULL; // １つ前のSendMessage関数。

int Lock_SendMessageA = FALSE;

// 元のSendMessageAと全く同じ形
extern "C" __declspec(dllexport) LRESULT WINAPI Hook_SendMessageA(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!Lock_SendMessageA) {
		Lock_SendMessageA = TRUE;

		//-------------------------------------------- ここから下はオリジナルを呼び出す処理
		// 文字コードの入力
		if (uMsg == 0x61E) {
			OutputDebugStream("☆☆☆SendmessageA %x, %x, %x", hWnd, wParam, lParam);
			OnHidemaruApplicationKeyDown(hWnd, wParam, lParam);
			wLastKeyDown = wParam;
		}
		/*
		if (hWnd == hWndAutoCompleteListBox) {
		if (Msg == LB_GETTEXT) {
		}
		}
		else if (hWnd == hWndAutoCompleteListBox) {
		OutputDebugStream("他 %x, %x, %x\n", Msg, wParam, lParam);
		}
		*/

	}
	Lock_SendMessageA = FALSE;
	return ((PFNSENDMESSAGEA)pfnPrevSendMessageA)(hWnd, uMsg, wParam, lParam);
}

map<int, string> stringmap;

// フックする関数のプロトタイプを定義
using PFNSENDMESSAGEW = LRESULT(WINAPI *)(HWND, UINT, WPARAM, LPARAM);

PROC pfnPrevSendMessageW = NULL; // １つ前のSendMessage関数。
int Lock_SendMessageW = FALSE;

// 元のSendMessageAと全く同じ形
extern "C" __declspec(dllexport) LRESULT WINAPI Hook_SendMessageW(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!Lock_SendMessageW) {
		Lock_SendMessageW = TRUE;

		//-------------------------------------------- ここから下はオリジナルを呼び出す処理
		// 文字コードの入力
		if (uMsg == 0x61E) {
			OutputDebugStream("☆☆☆SendmessageW %x, %x, %x", hWnd, wParam, lParam);
			OnHidemaruApplicationKeyDown(hWnd, wParam, lParam);
			wLastKeyDown = wParam;
		}
		if (stringmap[0x0180] == "") {
			stringmap[0x0180] = "LB_ADDSTRING";
			stringmap[0x0181] = "LB_INSERTSTRING";
			stringmap[0x0182] = "LB_DELETESTRING";
			stringmap[0x0183] = "LB_SELITEMRANGEEX";
			stringmap[0x0184] = "LB_RESETCONTENT";
			stringmap[0x0185] = "LB_SETSEL";
			stringmap[0x0186] = "LB_SETCURSEL";
			stringmap[0x0187] = "LB_GETSEL";
			stringmap[0x0188] = "LB_GETCURSEL";
			stringmap[0x0189] = "LB_GETTEXT";
			stringmap[0x018A] = "LB_GETTEXTLEN";
			stringmap[0x018B] = "LB_GETCOUNT";
			stringmap[0x018C] = "LB_SELECTSTRING";
			stringmap[0x018D] = "LB_DIR";
			stringmap[0x018E] = "LB_GETTOPINDEX";
			stringmap[0x018F] = "LB_FINDSTRING";
			stringmap[0x0190] = "LB_GETSELCOUNT";
			stringmap[0x0191] = "LB_GETSELITEMS";
			stringmap[0x0192] = "LB_SETTABSTOPS";
			stringmap[0x0193] = "LB_GETHORIZONTALEXTENT";
			stringmap[0x0194] = "LB_SETHORIZONTALEXTENT";
			stringmap[0x0195] = "LB_SETCOLUMNWIDTH";
			stringmap[0x0196] = "LB_ADDFILE";
			stringmap[0x0197] = "LB_SETTOPINDEX";
			stringmap[0x0198] = "LB_GETITEMRECT";
			stringmap[0x0199] = "LB_GETITEMDATA";
			stringmap[0x019A] = "LB_SETITEMDATA";
			stringmap[0x019B] = "LB_SELITEMRANGE";
			stringmap[0x019C] = "LB_SETANCHORINDEX";
			stringmap[0x019D] = "LB_GETANCHORINDEX";
			stringmap[0x019E] = "LB_SETCARETINDEX";
			stringmap[0x019F] = "LB_GETCARETINDEX";
			stringmap[0x01A0] = "LB_SETITEMHEIGHT";
			stringmap[0x01A1] = "LB_GETITEMHEIGHT";
			stringmap[0x01A2] = "LB_FINDSTRINGEXACT";
			stringmap[0x01A5] = "LB_SETLOCALE";
			stringmap[0x01A6] = "LB_GETLOCALE";
			stringmap[0x01A7] = "LB_SETCOUNT";
			stringmap[0x01A8] = "LB_INITSTORAGE";
			stringmap[0x01A9] = "LB_ITEMFROMPOINT";
		}
		if (hWnd == hWndAutoCompleteListBox) {
			/*
			PFNPROC curproc = (PFNPROC)GetWindowLong(hWndAutoCompleteListBox, GWL_WNDPROC);
			if (curproc != WindowProcAutoCompleteListBox) {
			pOriginalWIndowProcAutoCompleteListBox = curproc;
			SetWindowLong(hWndAutoCompleteListBox, GWL_WNDPROC, (LONG)WindowProcAutoCompleteListBox);
			}
			*/
			/*
			0x0180 LB_ADDSTRING            0x0180
			0x0181 LB_INSERTSTRING         0x0181
			0x0182 LB_DELETESTRING         0x0182
			0x0183 LB_SELITEMRANGEEX       0x0183
			0x0184 LB_RESETCONTENT         0x0184
			0x0185 LB_SETSEL               0x0185
			0x0186 LB_SETCURSEL            0x0186
			0x0187 LB_GETSEL               0x0187
			0x0188 LB_GETCURSEL            0x0188
			0x0189 LB_GETTEXT              0x0189
			0x018A LB_GETTEXTLEN           0x018A
			0x018B LB_GETCOUNT             0x018B
			0x018C LB_SELECTSTRING         0x018C
			0x018D LB_DIR                  0x018D
			0x018E LB_GETTOPINDEX          0x018E
			0x018F LB_FINDSTRING           0x018F
			0x0190 LB_GETSELCOUNT          0x0190
			0x0191 LB_GETSELITEMS          0x0191
			0x0192 LB_SETTABSTOPS          0x0192
			0x0193 LB_GETHORIZONTALEXTENT  0x0193
			0x0194 LB_SETHORIZONTALEXTENT  0x0194
			0x0195 LB_SETCOLUMNWIDTH       0x0195
			0x0196 LB_ADDFILE              0x0196
			0x0197 LB_SETTOPINDEX          0x0197
			0x0198 LB_GETITEMRECT          0x0198
			0x0199 LB_GETITEMDATA          0x0199
			0x019A LB_SETITEMDATA          0x019A
			0x019B LB_SELITEMRANGE         0x019B
			0x019C LB_SETANCHORINDEX       0x019C
			0x019D LB_GETANCHORINDEX       0x019D
			0x019E LB_SETCARETINDEX        0x019E
			0x019F LB_GETCARETINDEX        0x019F
			0x01A0 LB_SETITEMHEIGHT        0x01A0
			0x01A1 LB_GETITEMHEIGHT        0x01A1
			0x01A2 LB_FINDSTRINGEXACT      0x01A2
			0x01A5 LB_SETLOCALE            0x01A5
			0x01A6 LB_GETLOCALE            0x01A6
			0x01A7 LB_SETCOUNT             0x01A7
			0x01A8 LB_INITSTORAGE          0x01A8
			0x01A9 LB_ITEMFROMPOINT        0x01A9
			*/
			/*
			if (uMsg == LBN_KILLFOCUS) {
			OutputDebugStream("★★入力補完からキーボードフォーカスを失った");
			}
			*/
			// 追加が始まった
			if (uMsg == LB_INSERTSTRING || uMsg == LB_ADDSTRING) {
				bAutoCompleteListBoxAddMode = 1;
			}
			// 追加中のフラグが立っているのに、追加命令が着ていない
			else if (bAutoCompleteListBoxAddMode == 1) {
				OnHidemaruListBoxAddComplete(hWndAutoCompleteListBox, uMsg, wParam, lParam);
				bAutoCompleteListBoxAddMode = 2;
			}
			if (uMsg == LB_GETTEXT && bAutoCompleteListBoxAddMode > 1 || uMsg == LB_GETTEXTLEN && bAutoCompleteListBoxAddMode && wParam == 0) {
				int cursor_index = ((PFNSENDMESSAGEA)pfnPrevSendMessageA)(hWnd, LB_GETCURSEL, 0, 0);
				int carret_index = ((PFNSENDMESSAGEA)pfnPrevSendMessageA)(hWnd, LB_GETCARETINDEX, 0, 0);

				// 選ばれていない
				if (cursor_index == -1) {
					/*
					// wParamが0より大きな値なら、選んでないのではなく、最後の項目を選択しているかもしれない
					if (wParam > 0) {
					// 個数を調べて
					int allcount = ((PFNSENDMESSAGEA)pfnPrevSendMessageA)(hWnd, LB_GETCOUNT, 0, 0);
					// wParamと個数-1が同じなら、最後の値を選択している
					if (wParam == allcount - 1) {
					// 最後の値を指示していることと修正する。
					cursor_index = allcount - 1;
					}
					}
					*/
				}

				if (cursor_index == -1) {
					;
				}
				else {
					char szBuffer[4096];
					((PFNSENDMESSAGEA)pfnPrevSendMessageA)(hWnd, LB_GETTEXT, cursor_index, (LPARAM)szBuffer);
					OutputDebugStream("ヘルプチップのタイミング!!%x, %x, %x, カーソル%d, カレット%d\n", uMsg, wParam, lParam, cursor_index, carret_index);

					RECT rect;
					GetWindowRect(hWnd, &rect);  // ウィンドウの座標値
					OutputDebugStream("■■■■座標%d,%d,%d,%d\n", rect.left, rect.top, rect.bottom, rect.right);

					int iItemHeight = ((PFNSENDMESSAGEA)pfnPrevSendMessageA)(hWnd, LB_GETITEMHEIGHT, 0, 0);

					if (cursor_index == 0) {
						// 改行+スペースみたいなのを除去
						Matches m;
						if (OnigMatch(szBuffer, "^(.+)   \\(.+?=.+?\\)$", &m)) {
							if (m[1].size() > 0) {
								strcpy(szBuffer, m[1].data()); // m[1]は部分文字列なので、絶対バッファに余裕がある。
							}
						}
					}

					AutoCompleteHelpTip::OnListBoxSelectedIndexChanged(hWnd, cursor_index, szBuffer, rect, iItemHeight);
				}
			}

			else if (stringmap[uMsg].size() > 1) {
				OutputDebugStream("コマンド ☆%s\n", stringmap[uMsg].data());
				OutputDebugStream("コマンド %x, %x, %x\n", uMsg, wParam, lParam);
			}
			else {
				OutputDebugStream("他 ☆%x\n", uMsg);
				OutputDebugStream("他 %x, %x, %x\n", uMsg, wParam, lParam);
			}
		}
		else if (hWnd == hWndAutoCompleteListBox) {
			OutputDebugStream("他 %x, %x, %x\n", uMsg, wParam, lParam);
		}
	}

	Lock_SendMessageW = FALSE;
	return ((PFNSENDMESSAGEW)pfnPrevSendMessageW)(hWnd, uMsg, wParam, lParam);
}
//-----------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------


// フックする関数のプロトタイプを定義
using PFNCREATEWINDOWEXA = HWND (WINAPI *)(DWORD dwExStyle,      // 拡張ウィンドウスタイル
	LPCTSTR lpClassName,  // 登録されているクラス名
	LPCTSTR lpWindowName, // ウィンドウ名
	DWORD dwStyle,        // ウィンドウスタイル
	int x,                // ウィンドウの横方向の位置
	int y,                // ウィンドウの縦方向の位置
	int nWidth,           // ウィンドウの幅
	int nHeight,          // ウィンドウの高さ
	HWND hWndParent,      // 親ウィンドウまたはオーナーウィンドウのハンドル
	HMENU hMenu,          // メニューハンドルまたは子識別子
	HINSTANCE hInstance,  // アプリケーションインスタンスのハンドル
	LPVOID lpParam        // ウィンドウ作成データ
);

PROC pfnPrevCreateWindowExA = NULL; // １つ前のSendMessage関数。

int Lock_CreateWindowExA = FALSE;

// 元のSendMessageAと全く同じ形
extern "C" __declspec(dllexport) HWND WINAPI Hook_CreateWindowExA(DWORD dwExStyle,      // 拡張ウィンドウスタイル
	LPCTSTR lpClassName,  // 登録されているクラス名
	LPCTSTR lpWindowName, // ウィンドウ名
	DWORD dwStyle,        // ウィンドウスタイル
	int x,                // ウィンドウの横方向の位置
	int y,                // ウィンドウの縦方向の位置
	int nWidth,           // ウィンドウの幅
	int nHeight,          // ウィンドウの高さ
	HWND hWndParent,      // 親ウィンドウまたはオーナーウィンドウのハンドル
	HMENU hMenu,          // メニューハンドルまたは子識別子
	HINSTANCE hInstance,  // アプリケーションインスタンスのハンドル
	LPVOID lpParam        // ウィンドウ作成データ
	)
{
	//-------------------------------------------- ここから下はオリジナルを呼び出す処理
	HWND ret = ((PFNCREATEWINDOWEXA)pfnPrevCreateWindowExA)(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

	if (!Lock_CreateWindowExA) {
		Lock_CreateWindowExA = TRUE;

		HWND parent = GetParent(ret);
		if (parent) {
			char sz[100];
			GetClassName(parent, sz, sizeof(sz));
			if (strcmp(sz, "HidemaruAutocomp") == 0) {
				hWndAutoCompleteListBox = ret;
				OutputDebugStream("★Aの方\n");
				GetClassName(ret, sz, sizeof(sz));

				OutputDebugStream(sz);
			}
		}
	}
	Lock_CreateWindowExA = FALSE;
	return ret;
}
//-----------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------


// フックする関数のプロトタイプを定義
using PFNCREATEWINDOWEXW = HWND(WINAPI *)(
	DWORD dwExStyle,      // 拡張ウィンドウスタイル
	LPCTSTR lpClassName,  // 登録されているクラス名
	LPCTSTR lpWindowName, // ウィンドウ名
	DWORD dwStyle,        // ウィンドウスタイル
	int x,                // ウィンドウの横方向の位置
	int y,                // ウィンドウの縦方向の位置
	int nWidth,           // ウィンドウの幅
	int nHeight,          // ウィンドウの高さ
	HWND hWndParent,      // 親ウィンドウまたはオーナーウィンドウのハンドル
	HMENU hMenu,          // メニューハンドルまたは子識別子
	HINSTANCE hInstance,  // アプリケーションインスタンスのハンドル
	LPVOID lpParam        // ウィンドウ作成データ
	);

PROC pfnPrevCreateWindowExW = NULL; // １つ前のSendMessage関数。

int Lock_CreateWindowExW = FALSE;

// 元のSendMessageAと全く同じ形
extern "C" __declspec(dllexport) HWND WINAPI Hook_CreateWindowExW(DWORD dwExStyle,      // 拡張ウィンドウスタイル
	LPCTSTR lpClassName,  // 登録されているクラス名
	LPCTSTR lpWindowName, // ウィンドウ名
	DWORD dwStyle,        // ウィンドウスタイル
	int x,                // ウィンドウの横方向の位置
	int y,                // ウィンドウの縦方向の位置
	int nWidth,           // ウィンドウの幅
	int nHeight,          // ウィンドウの高さ
	HWND hWndParent,      // 親ウィンドウまたはオーナーウィンドウのハンドル
	HMENU hMenu,          // メニューハンドルまたは子識別子
	HINSTANCE hInstance,  // アプリケーションインスタンスのハンドル
	LPVOID lpParam        // ウィンドウ作成データ
	)
{
	//-------------------------------------------- ここから下はオリジナルを呼び出す処理
	HWND ret = ((PFNCREATEWINDOWEXA)pfnPrevCreateWindowExW)(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
	if (!Lock_CreateWindowExW) {
		Lock_CreateWindowExW = TRUE;

		HWND parent = GetParent(ret);
		if (parent) {
			char sz[100];
			GetClassName(parent, sz, sizeof(sz));
			if (strcmp(sz, "HidemaruAutocomp") == 0) {
				hWndAutoCompleteListBox = ret;
				OutputDebugStream("★Wの方\n");
				GetClassName(ret, sz, sizeof(sz));
				OutputDebugStream(sz);
				OnHidemaruListBoxAppear(hWndAutoCompleteListBox, wLastKeyDown);
			}
		}
	}

	Lock_CreateWindowExW = FALSE;

	return ret;
}
//-----------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------



// フックする関数のプロトタイプを定義
using PFNDESTROYWINDOW = BOOL(WINAPI *)(
	HWND hWnd  // 破棄するウィンドウのハンドル
);

PROC pfnPrevDestroyWindow = NULL; // １つ前のDestroyWindow関数。

int Lock_DestroyWindow = FALSE;

// 元のSendMessageAと全く同じ形
extern "C" __declspec(dllexport) BOOL WINAPI Hook_DestroyWindow(HWND hWnd)  // 破棄するウィンドウのハンドル
	
{
	if (!Lock_DestroyWindow) {
		Lock_DestroyWindow = TRUE;

		char cname[100];
		GetClassName(hWnd, cname, sizeof(cname));
		if (hWnd == hWndAutoCompleteListBox || strcmp(cname, "HidemaruAutocomp") == 0) {

			// 自動補完時のヘルプチップの準備
			AutoCompleteHelpTip::OnDestroy(hWndAutoCompleteListBox);

			hWndAutoCompleteListBox = NULL;
			ResetAutoCompleteListBoxIndex();
			OutputDebugStream("★★DestroyAutoCompleteWindow\n");
		}
	}

	Lock_DestroyWindow = FALSE;
	//-------------------------------------------- ここから下はオリジナルを呼び出す処理
	return ((PFNDESTROYWINDOW)pfnPrevDestroyWindow)(hWnd);
}
//-----------------------------------------------------------------------------------------------------------------------------------
/*
// フックする関数のプロトタイプを定義
using PFNCREATEFILEA = HANDLE (WINAPI *)(
	LPCTSTR lpFileName,                         // ファイル名
	DWORD dwDesiredAccess,                      // アクセスモード
	DWORD dwShareMode,                          // 共有モード
	LPSECURITY_ATTRIBUTES lpSecurityAttributes, // セキュリティ記述子
	DWORD dwCreationDisposition,                // 作成方法
	DWORD dwFlagsAndAttributes,                 // ファイル属性
	HANDLE hTemplateFile                        // テンプレートファイルのハンドル
	);

PROC pfnPrevCreateFileA = NULL; // １つ前のDestroyWindow関数。
// 元のSendMessageAと全く同じ形
extern "C" __declspec(dllexport) HANDLE WINAPI Hook_CreateFileA(
	LPCTSTR lpFileName,                         // ファイル名
	DWORD dwDesiredAccess,                      // アクセスモード
	DWORD dwShareMode,                          // 共有モード
	LPSECURITY_ATTRIBUTES lpSecurityAttributes, // セキュリティ記述子
	DWORD dwCreationDisposition,                // 作成方法
	DWORD dwFlagsAndAttributes,                 // ファイル属性
	HANDLE hTemplateFile                        // テンプレートファイルのハンドル
	)
{
	OutputDebugStream("■CreateFileA");
	OutputDebugStream("%s", lpFileName);
	OutputDebugStream("\n");
	//-------------------------------------------- ここから下はオリジナルを呼び出す処理
	HANDLE ret = ((PFNCREATEFILEA)pfnPrevCreateFileA)(
			lpFileName,
			dwDesiredAccess,
			dwShareMode,
			lpSecurityAttributes,
			dwCreationDisposition,
			dwFlagsAndAttributes,
			hTemplateFile);
	return ret;
}



// フックする関数のプロトタイプを定義
using PFNCREATEFILEW = HANDLE(WINAPI *)(
	LPCTSTR lpFileName,                         // ファイル名
	DWORD dwDesiredAccess,                      // アクセスモード
	DWORD dwShareMode,                          // 共有モード
	LPSECURITY_ATTRIBUTES lpSecurityAttributes, // セキュリティ記述子
	DWORD dwCreationDisposition,                // 作成方法
	DWORD dwFlagsAndAttributes,                 // ファイル属性
	HANDLE hTemplateFile                        // テンプレートファイルのハンドル
	);

PROC pfnPrevCreateFileW = NULL; // １つ前のDestroyWindow関数。
// 元のSendMessageAと全く同じ形
extern "C" __declspec(dllexport) HANDLE WINAPI Hook_CreateFileW(
	LPCTSTR lpFileName,                         // ファイル名
	DWORD dwDesiredAccess,                      // アクセスモード
	DWORD dwShareMode,                          // 共有モード
	LPSECURITY_ATTRIBUTES lpSecurityAttributes, // セキュリティ記述子
	DWORD dwCreationDisposition,                // 作成方法
	DWORD dwFlagsAndAttributes,                 // ファイル属性
	HANDLE hTemplateFile                        // テンプレートファイルのハンドル
	)
{
	OutputDebugStream("■CreateFileW");

	//変換文字列格納バッファ
	char	wStrC[512];

	size_t wLen = 0;
	errno_t err = 0;

	//変換
	err = wcstombs_s(&wLen, wStrC, 512, (const wchar_t *)lpFileName, _TRUNCATE);

	OutputDebugStream("%s", wStrC);
	OutputDebugStream("\n");
	//-------------------------------------------- ここから下はオリジナルを呼び出す処理
	HANDLE ret = ((PFNCREATEFILEA)pfnPrevCreateFileW)(
		lpFileName,
		dwDesiredAccess,
		dwShareMode,
		lpSecurityAttributes,
		dwCreationDisposition,
		dwFlagsAndAttributes,
		hTemplateFile);
	return ret;
}
*/

BOOL HookWin32apiFuncs_Done = FALSE;
void HookWin32apiFuncs() {

	if (!HookWin32apiFuncs_Done) {
		HookWin32apiFuncs_Done = TRUE;

		pfnPrevSendMessageA    = hook_win32api("user32.dll", "SendMessageA", Hook_SendMessageA);
		pfnPrevSendMessageW    = hook_win32api("user32.dll", "SendMessageW", Hook_SendMessageW);
		pfnPrevCreateWindowExA = hook_win32api("user32.dll", "CreateWindowExA", Hook_CreateWindowExA);
		pfnPrevCreateWindowExW = hook_win32api("user32.dll", "CreateWindowExW", Hook_CreateWindowExW);
		pfnPrevDestroyWindow   = hook_win32api("user32.dll", "DestroyWindow", Hook_DestroyWindow);
		// pfnPrevCreateFileA     = hook_win32api("kernel32.dll", "CreateFileA", Hook_CreateFileA);
		// pfnPrevCreateFileW     = hook_win32api("kernel32.dll", "CreateFileW", Hook_CreateFileW);
	}
}