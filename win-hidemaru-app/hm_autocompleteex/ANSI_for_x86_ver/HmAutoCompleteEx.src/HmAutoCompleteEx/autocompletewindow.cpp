#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <vector>
#include <algorithm>

#include "outputdebugstream.h"
#include "autocompletewindow.h"
#include "hookwin32api.h"
#include "programmemory.h"
#include "assemblerjumper.h"
#include "autocomletehelptips.h"

#include "OnigRegex.h"

using namespace std;

HWND hWndAutoCompleteListBox = NULL;
extern WPARAM wLastKeyDown = 0;

int iPrevAutoCompleteListBoxIndex = -1;
BOOL bAutoCompleteListBoxAddMode = 0;
BOOL ResetAutoCompleteListBoxIndex() {
	iPrevAutoCompleteListBoxIndex = -1;
	bAutoCompleteListBoxAddMode = 0;
	return TRUE;
}

// 秀丸の入力補完用のリストボックス(的なもの含む)が出現する直前
BOOL OnHidemaruListBoxAppear(HWND hWnd, WPARAM wparam) {
	// 特定のリストボックス内のインデックスを指し示していた値をリセット
	ResetAutoCompleteListBoxIndex();
	OutputDebugStream("リストボックスの出現");

	// 自動補完時のヘルプチップの準備
	AutoCompleteHelpTip::OnCreate(hWnd, szCurrentFileOnCallSetHidemaruHandle);


	return 0;
}

BOOL OnHidemaruListBoxKeyDown(HWND hWnd, WPARAM wParam) {
	if (hWndAutoCompleteListBox) {
		OutputDebugStream("リストボックスの更新");
		int iGetCurSel = ((PFNSENDMESSAGEA)pfnPrevSendMessageA)(hWndAutoCompleteListBox, LB_GETCURSEL, 0, 0);

		OutputDebugStream("項目数 = %d\n項目文字数 = %d\n",
			((PFNSENDMESSAGEA)pfnPrevSendMessageA)(hWndAutoCompleteListBox, LB_GETCOUNT, 0, 0),
			((PFNSENDMESSAGEA)pfnPrevSendMessageA)(hWndAutoCompleteListBox, LB_GETTEXTLEN, iGetCurSel, 0)
			);

	}

	return 0;
}

BOOL OnHidemaruApplicationKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam) {
	OutputDebugStream("SendmessageW %c", wParam);
	OnHidemaruListBoxKeyDown(hWnd, wParam);
	AutoCompleteHelpTip::OnKeyDown(hWnd, wParam, lParam);
	return true;
}

extern BOOL AddCompleteString(HWND hListBox, WPARAM wParam);

PFNPROC pOriginalWIndowProcAutoCompleteListBox = NULL;
LRESULT CALLBACK WindowProcAutoCompleteListBox(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (uMsg == LB_GETTEXT) {
		OutputDebugStream("★★ PROC内 %d", wParam);
	}

	return CallWindowProc(
		pOriginalWIndowProcAutoCompleteListBox,  // 元のウィンドウプロシージャ
		hWnd,              // ウィンドウのハンドル
		uMsg,               // メッセージ
		wParam,          // メッセージの最初のパラメータ
		lParam           // メッセージの 2 番目のパラメータ
	);
}


BOOL OnHidemaruListBoxAddComplete(HWND hListBox, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	OutputDebugStream("☆追加が終わった疑い\n");

	int len = ((PFNSENDMESSAGEA)pfnPrevSendMessageA)(hListBox, LB_GETCOUNT, 0, 0);
	char sz[4096] = ""; // 秀丸の一行の最大文字数
	vector<string> original_list;
	for (int i = 0; i < len; i++) {
		((PFNSENDMESSAGEA)pfnPrevSendMessageA)(hListBox, LB_GETTEXT, i, (LPARAM)sz);
		if (i == 0) {
			// 改行+スペースみたいなのを除去
			Matches m;
			if (OnigMatch(sz, "^(.+)   \\(.+?=.+?\\)$", &m)) {
				if (m[1].size() > 0) {
					strcpy(sz, m[1].data()); // m[1]は部分文字列なので、絶対バッファに余裕がある。
				}
			}

		}
		original_list.push_back(string(sz));
	}

	auto addition_list = AutoCompleteHelpTip::OnQueryListBoxCustomAdded(hListBox, original_list, szCurrentFileOnCallSetHidemaruHandle);
	std::reverse(addition_list.begin(), addition_list.end());

	// ここ回転数多いので、コピーすくなく
	for (string& curnew : addition_list) {
		int exist = false;

		for (string& curorg : original_list) {
			if (curnew == curorg) {
				exist = true;
				break;
			}
		}

		// 無い場合だけ足す
		if (!exist) {
			((PFNSENDMESSAGEA)pfnPrevSendMessageA)(hListBox, LB_INSERTSTRING, 1, (LPARAM)curnew.data());
			// ((PFNSENDMESSAGEA)pfnPrevSendMessageA)(hListBox, LB_INSERTSTRING, 0, (LPARAM)curnew.data());
		}

	}

	return TRUE;
}


