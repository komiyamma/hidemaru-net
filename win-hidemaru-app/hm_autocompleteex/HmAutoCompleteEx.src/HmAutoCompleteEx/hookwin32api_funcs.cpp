#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <regex>
#include <map>

#include "autocompletewindow.h"
#include "outputdebugstream.h"
#include "hookwin32api.h"
#include "autocomletehelptips.h"
#include "tstring.h"



using namespace std;




// フックする関数のプロトタイプを定義
using PFNSENDMESSAGEA = LRESULT(WINAPI *)(HWND, UINT, WPARAM, LPARAM);

PROC pfnPrevSendMessageA = NULL; // １つ前のSendMessage関数。

int Lock_SendMessageA = FALSE;

// 元のSendMessageAと全く同じ形
LRESULT WINAPI Hook_SendMessageA(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!Lock_SendMessageA) {
		Lock_SendMessageA = TRUE;

		//-------------------------------------------- ここから下はオリジナルを呼び出す処理
		// 文字コードの入力
		if (uMsg == 0x61E) {
			OutputDebugStream(_T("☆☆☆SendmessageA %x, %x, %x"), hWnd, wParam, lParam);
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

map<int, tstring> stringmap;

// フックする関数のプロトタイプを定義
using PFNSENDMESSAGEW = LRESULT(WINAPI *)(HWND, UINT, WPARAM, LPARAM);

PROC pfnPrevSendMessageW = NULL; // １つ前のSendMessage関数。
int Lock_SendMessageW = FALSE;

// 元のSendMessageAと全く同じ形
LRESULT WINAPI Hook_SendMessageW(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!Lock_SendMessageW) {
		Lock_SendMessageW = TRUE;

		//-------------------------------------------- ここから下はオリジナルを呼び出す処理
		// 文字コードの入力
		if (uMsg == 0x61E) {
			OutputDebugStream(_T("☆☆☆SendmessageW %x, %x, %x"), hWnd, wParam, lParam);
			OnHidemaruApplicationKeyDown(hWnd, wParam, lParam);
			wLastKeyDown = wParam;
		}
		if (stringmap[0x0180] == _T("")) {
			stringmap[0x0180] = _T("LB_ADDSTRING");
			stringmap[0x0181] = _T("LB_INSERTSTRING");
			stringmap[0x0182] = _T("LB_DELETESTRING");
			stringmap[0x0183] = _T("LB_SELITEMRANGEEX");
			stringmap[0x0184] = _T("LB_RESETCONTENT");
			stringmap[0x0185] = _T("LB_SETSEL");
			stringmap[0x0186] = _T("LB_SETCURSEL");
			stringmap[0x0187] = _T("LB_GETSEL");
			stringmap[0x0188] = _T("LB_GETCURSEL");
			stringmap[0x0189] = _T("LB_GETTEXT");
			stringmap[0x018A] = _T("LB_GETTEXTLEN");
			stringmap[0x018B] = _T("LB_GETCOUNT");
			stringmap[0x018C] = _T("LB_SELECTSTRING");
			stringmap[0x018D] = _T("LB_DIR");
			stringmap[0x018E] = _T("LB_GETTOPINDEX");
			stringmap[0x018F] = _T("LB_FINDSTRING");
			stringmap[0x0190] = _T("LB_GETSELCOUNT");
			stringmap[0x0191] = _T("LB_GETSELITEMS");
			stringmap[0x0192] = _T("LB_SETTABSTOPS");
			stringmap[0x0193] = _T("LB_GETHORIZONTALEXTENT");
			stringmap[0x0194] = _T("LB_SETHORIZONTALEXTENT");
			stringmap[0x0195] = _T("LB_SETCOLUMNWIDTH");
			stringmap[0x0196] = _T("LB_ADDFILE");
			stringmap[0x0197] = _T("LB_SETTOPINDEX");
			stringmap[0x0198] = _T("LB_GETITEMRECT");
			stringmap[0x0199] = _T("LB_GETITEMDATA");
			stringmap[0x019A] = _T("LB_SETITEMDATA");
			stringmap[0x019B] = _T("LB_SELITEMRANGE");
			stringmap[0x019C] = _T("LB_SETANCHORINDEX");
			stringmap[0x019D] = _T("LB_GETANCHORINDEX");
			stringmap[0x019E] = _T("LB_SETCARETINDEX");
			stringmap[0x019F] = _T("LB_GETCARETINDEX");
			stringmap[0x01A0] = _T("LB_SETITEMHEIGHT");
			stringmap[0x01A1] = _T("LB_GETITEMHEIGHT");
			stringmap[0x01A2] = _T("LB_FINDSTRINGEXACT");
			stringmap[0x01A5] = _T("LB_SETLOCALE");
			stringmap[0x01A6] = _T("LB_GETLOCALE");
			stringmap[0x01A7] = _T("LB_SETCOUNT");
			stringmap[0x01A8] = _T("LB_INITSTORAGE");
			stringmap[0x01A9] = _T("LB_ITEMFROMPOINT");
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
				if (bAutoCompleteListBoxAddMode == 0) {
					OnHidemaruListBoxAddCompletePrev(hWndAutoCompleteListBox, uMsg, wParam, lParam);
				}
				bAutoCompleteListBoxAddMode = 1;
			}
			// 追加中のフラグが立っているのに、追加命令が着ていない
			else if (bAutoCompleteListBoxAddMode == 1) {
				OnHidemaruListBoxAddCompletePost(hWndAutoCompleteListBox, uMsg, wParam, lParam);
				bAutoCompleteListBoxAddMode = 2;
			}
			if (uMsg == LB_GETTEXT && bAutoCompleteListBoxAddMode > 1 || uMsg == LB_GETTEXTLEN && bAutoCompleteListBoxAddMode && wParam == 0) {
				LRESULT cursor_index = ((PFNSENDMESSAGEA)pfnPrevSendMessageA)(hWnd, LB_GETCURSEL, 0, 0);
				LRESULT carret_index = ((PFNSENDMESSAGEA)pfnPrevSendMessageA)(hWnd, LB_GETCARETINDEX, 0, 0);

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
					TCHAR szBuffer[4096];
					((PFNSENDMESSAGEA)pfnPrevSendMessageW)(hWnd, LB_GETTEXT, cursor_index, (LPARAM)szBuffer);
					OutputDebugStream(_T("ヘルプチップのタイミング!!%x, %x, %x, カーソル%d, カレット%d\n"), uMsg, wParam, lParam, cursor_index, carret_index);

					RECT rect;
					GetWindowRect(hWnd, &rect);  // ウィンドウの座標値
					OutputDebugStream(_T("■■■■座標%d,%d,%d,%d\n"), rect.left, rect.top, rect.bottom, rect.right);

					LRESULT iItemHeight = ((PFNSENDMESSAGEA)pfnPrevSendMessageA)(hWnd, LB_GETITEMHEIGHT, 0, 0);

					if (cursor_index == 0) {
						// 改行+スペースみたいなのを除去
						tcmatch m;
						tregex re(AUTOCOMPLETE_HIDEMARU_ESPECIAL_ADD_WORD_REGEX);
						tstring result = regex_replace(szBuffer, re, _T(""));
						_tcscpy(szBuffer, result.data());
					}

					AutoCompleteHelpTip::OnListBoxSelectedIndexChanged(hWnd, (int)cursor_index, szBuffer, rect, (int)iItemHeight);
				}
			}

			else if (stringmap[uMsg].size() > 1) {
				OutputDebugStream(_T("コマンド ☆%s\n"), stringmap[uMsg].data());
				OutputDebugStream(_T("コマンド %x, %x, %x\n"), uMsg, wParam, lParam);
			}
			else {
				OutputDebugStream(_T("他 ☆%x\n"), uMsg);
				OutputDebugStream(_T("他 %x, %x, %x\n"), uMsg, wParam, lParam);
			}
		}
		else if (hWnd == hWndAutoCompleteListBox) {
			OutputDebugStream(_T("他 %x, %x, %x\n"), uMsg, wParam, lParam);
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
HWND WINAPI Hook_CreateWindowExA(DWORD dwExStyle,      // 拡張ウィンドウスタイル
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
			TCHAR sz[100];
			GetClassName(parent, sz, sizeof(sz));
			if (_tcscmp(sz, _T("HidemaruAutocomp")) == 0) {
				hWndAutoCompleteListBox = ret;
				OutputDebugStream(_T("★Aの方\n"));
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

HWND WINAPI Hook_CreateWindowExW(DWORD dwExStyle,      // 拡張ウィンドウスタイル
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
			TCHAR sz[100];
			GetClassName(parent, sz, sizeof(sz));
			if (_tcscmp(sz, _T("HidemaruAutocomp")) == 0) {
				hWndAutoCompleteListBox = ret;
				OutputDebugStream(_T("★Wの方\n"));
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
BOOL WINAPI Hook_DestroyWindow(HWND hWnd)  // 破棄するウィンドウのハンドル
	
{
	if (!Lock_DestroyWindow) {
		Lock_DestroyWindow = TRUE;

		TCHAR cname[100];
		GetClassName(hWnd, cname, sizeof(cname));
		if (hWnd == hWndAutoCompleteListBox || _tcscmp(cname, _T("HidemaruAutocomp") ) == 0) {

			// 自動補完時のヘルプチップの準備
			AutoCompleteHelpTip::OnDestroy(hWndAutoCompleteListBox);

			hWndAutoCompleteListBox = NULL;
			ResetAutoCompleteListBoxIndex();
			OutputDebugStream(_T("★★DestroyAutoCompleteWindow\n"));
		}
	}

	Lock_DestroyWindow = FALSE;
	//-------------------------------------------- ここから下はオリジナルを呼び出す処理
	return ((PFNDESTROYWINDOW)pfnPrevDestroyWindow)(hWnd);
}

BOOL HookWin32apiFuncs_Done = FALSE;
void HookWin32apiFuncs() {

	if (!HookWin32apiFuncs_Done) {
		HookWin32apiFuncs_Done = TRUE;

		pfnPrevSendMessageA    = hook_win32api("user32.dll", "SendMessageA", Hook_SendMessageA);
		pfnPrevSendMessageW    = hook_win32api("user32.dll", "SendMessageW", Hook_SendMessageW);
		pfnPrevCreateWindowExA = hook_win32api("user32.dll", "CreateWindowExA", Hook_CreateWindowExA);
		pfnPrevCreateWindowExW = hook_win32api("user32.dll", "CreateWindowExW", Hook_CreateWindowExW);
		pfnPrevDestroyWindow   = hook_win32api("user32.dll", "DestroyWindow", Hook_DestroyWindow);
	}
}