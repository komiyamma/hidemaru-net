/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <vector>
#include <algorithm>
#include "tstring.h"

#include "hookwin32api.h"
#include "outputdebugstream.h"
#include "autocompletewindow.h"
#include "sethidemarufile.h"
#include "autocomletehelptips.h"


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
	OutputDebugStream(_T("リストボックスの出現"));

	// 自動補完時のヘルプチップの準備
	AutoCompleteHelpTip::OnCreate(hWnd, GetCurrentFileName() );

	return 0;
}

BOOL OnHidemaruListBoxKeyDown(HWND hWnd, WPARAM wParam) {
	if (hWndAutoCompleteListBox) {
		OutputDebugStream(_T("リストボックスの更新"));
		LRESULT iGetCurSel = ((PFNSENDMESSAGEA)pfnPrevSendMessageA)(hWndAutoCompleteListBox, LB_GETCURSEL, 0, 0);

		OutputDebugStream(_T("項目数 = %d\n項目文字数 = %d\n"),
			((PFNSENDMESSAGEA)pfnPrevSendMessageA)(hWndAutoCompleteListBox, LB_GETCOUNT, 0, 0),
			((PFNSENDMESSAGEA)pfnPrevSendMessageA)(hWndAutoCompleteListBox, LB_GETTEXTLEN, iGetCurSel, 0)
			);

	}

	return 0;
}

BOOL OnHidemaruApplicationKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam) {
	OutputDebugStream(_T("SendmessageW %c"), wParam);
	OnHidemaruListBoxKeyDown(hWnd, wParam);
	AutoCompleteHelpTip::OnKeyDown(hWnd, wParam, lParam);
	return true;
}

extern BOOL AddCompleteString(HWND hListBox, WPARAM wParam);

PFNPROC pOriginalWIndowProcAutoCompleteListBox = NULL;
LRESULT CALLBACK WindowProcAutoCompleteListBox(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (uMsg == LB_GETTEXT) {
		OutputDebugStream(_T("★★ PROC内 %d"), wParam);
	}

	return CallWindowProc(
		pOriginalWIndowProcAutoCompleteListBox,  // 元のウィンドウプロシージャ
		hWnd,              // ウィンドウのハンドル
		uMsg,               // メッセージ
		wParam,          // メッセージの最初のパラメータ
		lParam           // メッセージの 2 番目のパラメータ
	);
}

BOOL OnHidemaruListBoxAddCompletePrev(HWND hListBox, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	OutputDebugStream(_T("☆これから追加の疑い\n"));
	// ((PFNSENDMESSAGEA)pfnPrevSendMessageW)(hListBox, LB_ADDSTRING, 1, (LPARAM)_T("テスト"));
	return TRUE;
}

BOOL OnHidemaruListBoxAddCompletePost(HWND hListBox, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	OutputDebugStream(_T("☆追加が終わった疑い\n"));

	LRESULT len = ((PFNSENDMESSAGEA)pfnPrevSendMessageW)(hListBox, LB_GETCOUNT, 0, 0);
	TCHAR sz[4096] = _T(""); // 秀丸の一行の最大文字数
	((PFNSENDMESSAGEA)pfnPrevSendMessageW)(hListBox, LB_GETTEXT, 0, (LPARAM)sz);
	OutputDebugStream(_T("最初の要素"));
	OutputDebugStream(sz);

	// 一番最初の要素に秀丸が勝手に修飾を付けてしまうのでカット開始
	tcmatch m;
	tregex re(AUTOCOMPLETE_HIDEMARU_ESPECIAL_ADD_WORD_REGEX);
	tstring result = regex_replace(sz, re, _T(""));
	_tcscpy(sz, result.data());
	OutputDebugStream(_T("マッチ1\n■"));
	OutputDebugStream(_T("結果発表:%s"), result.data());
	((PFNSENDMESSAGEA)pfnPrevSendMessageW)(hListBox, LB_DELETESTRING, 0, 0);
	((PFNSENDMESSAGEA)pfnPrevSendMessageW)(hListBox, LB_INSERTSTRING, 0, (LPARAM)sz);
	// 一番最初の要素に秀丸が勝手に修飾を付けてしまうのでカットここまで

	vector<tstring> original_list;
	for (int i = 0; i < len; i++) {
		((PFNSENDMESSAGEA)pfnPrevSendMessageW)(hListBox, LB_GETTEXT, i, (LPARAM)sz);
		OutputDebugStream(sz);
		original_list.push_back(tstring(sz));
	}

	auto addition_list = AutoCompleteHelpTip::OnQueryListBoxCustomAdded(hListBox, original_list);
	std::reverse(addition_list.begin(), addition_list.end());
	// ここ回転数多いので、コピーすくなく
	for (tstring& curnew : addition_list) {
		int exist = false;

		for (tstring& curorg : original_list) {
			if (curnew == curorg) {
				exist = true;
				break;
			}
		}

		// 無い場合だけ足す
		if (!exist) {
			((PFNSENDMESSAGEA)pfnPrevSendMessageW)(hListBox, LB_INSERTSTRING, 1, (LPARAM)curnew.data());
			// ((PFNSENDMESSAGEA)pfnPrevSendMessageW)(hListBox, LB_INSERTSTRING, 0, (LPARAM)curnew.data());
		}

	}

	return TRUE;
}


