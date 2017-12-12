/*
 * Copyright (C) 2016-2017 Akitsugu Komiyama and Microsoft Corporation
 * under the BSD 3-Clause License
 */


#define _CRT_SECURE_NO_WARNINGS
// TimerMessageBox.cpp : �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B

#include <windows.h>

#include <tchar.h>
#include <string>
#include <algorithm>

#include "nmh.h"
#include "utf8_to_sjis.h"

using namespace std;

// TimerMessageBox.cpp : �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

/***********************************************************************
��ʂɁA���Ԃ𐧌����ă��b�Z�[�W �{�b�N�X��\��������ꍇ�A
�w�肵�����Ԃ��o�߂�����Ŏ����I�ɕ���ʏ�̃_�C�A���O
�{�b�N�X����������K�v������܂��B

���̕��@�̖��_�́AWindows ���񋟂���W�����b�Z�[�W
�{�b�N�X�̋@�\�������Ă��܂����Ƃł��B

�ȉ��̗�ł́AMessageBox �֐����g�p���āA�w�肵�����Ԃ̌o�ߌ��
�����I�ɕ��郁�b�Z�[�W �{�b�N�X���쐬������@�������܂��B

���̗�Ɋւ��āA���̓_�ɒ��ӂ��Ă��������B
�E���̗�ł́A�w�肵�����Ԃ��o�߂�����ɃC�x���g�𔭐�������
�@Windows �^�C�}�[���g�p���܂��B
 �E�^�C�}�[ �C�x���g����������ƁAPostQuitMessage API ���g�p���āA
 �@MessageBox ���g�p���Ă��郂�[�_�� ���b�Z�[�W ���[�v�𔲂��܂��B
  �E�� : WM_QUIT �́A���C�� ���b�Z�[�W �L���[�Ŏ擾����Ȃ��悤�ɁA
  �@���b�Z�[�W �L���[����폜����K�v������܂��B

   �T���v�� �R�[�h
   �� : ���̃T���v���ł́A�T�[�r�X���� MessageBox ���Ăяo���ꍇ��
   MB_SERVICE_NOTIFICATION �t���O�̎g�p�̓T�|�[�g���Ă��܂���B
   MB_SERVICE_NOTIFICATION ���w�肳�ꂽ�ꍇ�A���b�Z�[�W �{�b�N�X�E�B���h�E�́A
   MessageBox ���Ăяo���v���Z�X�ɂ���ď��L����܂���B

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
	// 1����g���ĂȂ���n�̌x�����
	(void)hInstance, (void)hPrevInstance, (void)lpCmdLine, (void)nCmdShow;

	// �^�C�}�[���b�Z�[�W�{�b�N�X
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
			// �O�����l�Ȃ�A
			if (std::all_of(strFlags.begin(), strFlags.end(), isdigit)) {
				iFlags = std::stoi(strFlags);
			}
		}

		int iTimeOut = 3000;
		if (__argc >= 5) {
			string strTimeOut = __argv[4];
			// �O�����l�Ȃ�A
			if (std::all_of(strTimeOut.begin(), strTimeOut.end(), isdigit)) {
				iTimeOut = std::stoi(strTimeOut);
			}
		}

		TCHAR szMessage[4096] = _T("");
		_tcscpy(szMessage, __argv[1]);

		string checkstring = string(szMessage) + string(szTitle);

		// utf8�n�Ƃ������肪�o����Asjis��
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

