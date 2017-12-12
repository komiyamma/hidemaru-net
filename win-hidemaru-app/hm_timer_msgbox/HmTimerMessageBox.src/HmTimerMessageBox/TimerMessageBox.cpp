/*
 * Copyright (C) 2016-2017 Akitsugu Komiyama and Microsoft Corporation
 * under the BSD 3-Clause License
 */


#define _CRT_SECURE_NO_WARNINGS
// TimerMessageBox.cpp : アプリケーションのエントリ ポイントを定義します。

#include <windows.h>

#include <tchar.h>
#include <string>
#include <algorithm>

#include "nmh.h"
#include "utf8_to_sjis.h"

using namespace std;

// TimerMessageBox.cpp : アプリケーションのエントリ ポイントを定義します。
//

/***********************************************************************
一般に、時間を制限してメッセージ ボックスを表示させる場合、
指定した時間が経過した後で自動的に閉じる通常のダイアログ
ボックスを実装する必要があります。

この方法の問題点は、Windows が提供する標準メッセージ
ボックスの機能が失われてしまうことです。

以下の例では、MessageBox 関数を使用して、指定した時間の経過後に
自動的に閉じるメッセージ ボックスを作成する方法を示します。

この例に関して、次の点に注意してください。
・この例では、指定した時間が経過した後にイベントを発生させる
　Windows タイマーを使用します。
 ・タイマー イベントが発生すると、PostQuitMessage API を使用して、
 　MessageBox が使用しているモーダル メッセージ ループを抜けます。
  ・注 : WM_QUIT は、メイン メッセージ キューで取得されないように、
  　メッセージ キューから削除する必要があります。

   サンプル コード
   注 : このサンプルでは、サービスから MessageBox を呼び出す場合の
   MB_SERVICE_NOTIFICATION フラグの使用はサポートしていません。
   MB_SERVICE_NOTIFICATION が指定された場合、メッセージ ボックスウィンドウは、
   MessageBox を呼び出すプロセスによって所有されません。

   ***********************************************************************/



/***********************************************************************
THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

Copyright 1998 Microsoft Corporation.  All Rights Reserved.
***********************************************************************/


/***********************************************************************
*
*  Global variables
*
***********************************************************************/
HWND g_hwndTimedOwner;
BOOL g_bTimedOut;


/***********************************************************************
*
*  MessageBoxTimer
*
*      The timer callback function that posts the fake quit message.
*      This function causes the message box to exit because the message box
*      has determined that the application is exiting.
*
***********************************************************************/
void CALLBACK MessageBoxTimer(HWND hwnd, UINT uiMsg, UINT idEvent, DWORD dwTime)
{
	g_bTimedOut = TRUE;
	if (g_hwndTimedOwner)
		EnableWindow(g_hwndTimedOwner, TRUE);
	PostQuitMessage(0);
}


/***********************************************************************
*
*  TimedMessageBox
*
*      The same as the standard MessageBox, except that TimedMessageBox
*      also accepts a timeout. If the user does not respond within the
*      specified timeout, the value 0 is returned instead of one of the
*      ID* values.
*
***********************************************************************/
int TimedMessageBox(HWND hwndOwner, LPCTSTR pszMessage, LPCTSTR pszTitle, UINT flags, DWORD dwTimeout)
{
	UINT idTimer;
	int iResult;

	g_hwndTimedOwner = NULL;
	g_bTimedOut = FALSE;

	if (hwndOwner && IsWindowEnabled(hwndOwner))
		g_hwndTimedOwner = hwndOwner;

	//
	// Set a timer to dismiss the message box.
	idTimer = SetTimer(NULL, 0, dwTimeout, (TIMERPROC)MessageBoxTimer);

	iResult = MessageBox(hwndOwner, pszMessage, pszTitle, flags);

	//
	// Finished with the timer.
	KillTimer(NULL, idTimer);

	//
	// See if there is a WM_QUIT message in the queue if we timed out.
	// Eat the message so we do not quit the whole application.
	if (g_bTimedOut)
	{
		MSG msg;
		PeekMessage(&msg, NULL, WM_QUIT, WM_QUIT, PM_REMOVE);
		iResult = IDTIMEOUT;
	}

	return iResult;
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	// 1回も使ってないよ系の警告回避
	(void)hInstance, (void)hPrevInstance, (void)lpCmdLine, (void)nCmdShow;

	// タイマーメッセージボックス
	//--------------------------------------------------------------------------
	UINT uiResult = IDTIMEOUT;

	if (__argc >= 2) {
		setlocale(LC_CTYPE, "jpn");

		TCHAR szTitle[1024] = _T("");
		if (__argc >= 3) {
			_tcscpy(szTitle, __argv[2]);
		}

		int iFlags = 0;
		if (__argc >= 4) {
			string strFlags = __argv[3];
			// 前部数値なら、
			if (std::all_of(strFlags.begin(), strFlags.end(), isdigit)) {
				iFlags = std::stoi(strFlags);
			}
		}

		int iTimeOut = 3000;
		if (__argc >= 5) {
			string strTimeOut = __argv[4];
			// 前部数値なら、
			if (std::all_of(strTimeOut.begin(), strTimeOut.end(), isdigit)) {
				iTimeOut = std::stoi(strTimeOut);
			}
		}

		TCHAR szMessage[4096] = _T("");
		_tcscpy(szMessage, __argv[1]);

		string checkstring = string(szMessage) + string(szTitle);

		// utf8系という判定が出たら、sjisへ
		enum NMH_CHAR_CODE n = nmh_code_detect(checkstring.c_str(), strlen(checkstring.c_str()));
		const char *szname = NMH_CHAR_CODE_NAME_LIST[n];
		if (strstr(szname, "UTF8")) {
			string sjisMessage = utf8_to_cp932(szMessage);
			strcpy(szMessage, sjisMessage.c_str());
			string sjisTitle = utf8_to_cp932(szTitle);
			strcpy(szTitle, sjisTitle.c_str());
		}

		// Ask the user a question. Give the user 3 seconds to
		// answer the question.
		// NULL first parameter is important.
		uiResult = TimedMessageBox(NULL, szMessage, szTitle, iFlags, iTimeOut);
	}

	return uiResult;
}

