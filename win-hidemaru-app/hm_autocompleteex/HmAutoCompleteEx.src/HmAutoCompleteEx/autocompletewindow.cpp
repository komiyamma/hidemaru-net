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

// �G�ۂ̓��͕⊮�p�̃��X�g�{�b�N�X(�I�Ȃ��̊܂�)���o�����钼�O
BOOL OnHidemaruListBoxAppear(HWND hWnd, WPARAM wparam) {
	// ����̃��X�g�{�b�N�X���̃C���f�b�N�X���w�������Ă����l�����Z�b�g
	ResetAutoCompleteListBoxIndex();
	OutputDebugStream(_T("���X�g�{�b�N�X�̏o��"));

	// �����⊮���̃w���v�`�b�v�̏���
	AutoCompleteHelpTip::OnCreate(hWnd, GetCurrentFileName() );

	return 0;
}

BOOL OnHidemaruListBoxKeyDown(HWND hWnd, WPARAM wParam) {
	if (hWndAutoCompleteListBox) {
		OutputDebugStream(_T("���X�g�{�b�N�X�̍X�V"));
		LRESULT iGetCurSel = ((PFNSENDMESSAGEA)pfnPrevSendMessageA)(hWndAutoCompleteListBox, LB_GETCURSEL, 0, 0);

		OutputDebugStream(_T("���ڐ� = %d\n���ڕ����� = %d\n"),
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
		OutputDebugStream(_T("���� PROC�� %d"), wParam);
	}

	return CallWindowProc(
		pOriginalWIndowProcAutoCompleteListBox,  // ���̃E�B���h�E�v���V�[�W��
		hWnd,              // �E�B���h�E�̃n���h��
		uMsg,               // ���b�Z�[�W
		wParam,          // ���b�Z�[�W�̍ŏ��̃p�����[�^
		lParam           // ���b�Z�[�W�� 2 �Ԗڂ̃p�����[�^
	);
}

BOOL OnHidemaruListBoxAddCompletePrev(HWND hListBox, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	OutputDebugStream(_T("�����ꂩ��ǉ��̋^��\n"));
	// ((PFNSENDMESSAGEA)pfnPrevSendMessageW)(hListBox, LB_ADDSTRING, 1, (LPARAM)_T("�e�X�g"));
	return TRUE;
}

BOOL OnHidemaruListBoxAddCompletePost(HWND hListBox, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	OutputDebugStream(_T("���ǉ����I������^��\n"));

	LRESULT len = ((PFNSENDMESSAGEA)pfnPrevSendMessageW)(hListBox, LB_GETCOUNT, 0, 0);
	TCHAR sz[4096] = _T(""); // �G�ۂ̈�s�̍ő啶����
	((PFNSENDMESSAGEA)pfnPrevSendMessageW)(hListBox, LB_GETTEXT, 0, (LPARAM)sz);
	OutputDebugStream(_T("�ŏ��̗v�f"));
	OutputDebugStream(sz);

	// ��ԍŏ��̗v�f�ɏG�ۂ�����ɏC����t���Ă��܂��̂ŃJ�b�g�J�n
	tcmatch m;
	tregex re(AUTOCOMPLETE_HIDEMARU_ESPECIAL_ADD_WORD_REGEX);
	tstring result = regex_replace(sz, re, _T(""));
	_tcscpy(sz, result.data());
	OutputDebugStream(_T("�}�b�`1\n��"));
	OutputDebugStream(_T("���ʔ��\:%s"), result.data());
	((PFNSENDMESSAGEA)pfnPrevSendMessageW)(hListBox, LB_DELETESTRING, 0, 0);
	((PFNSENDMESSAGEA)pfnPrevSendMessageW)(hListBox, LB_INSERTSTRING, 0, (LPARAM)sz);
	// ��ԍŏ��̗v�f�ɏG�ۂ�����ɏC����t���Ă��܂��̂ŃJ�b�g�����܂�

	vector<tstring> original_list;
	for (int i = 0; i < len; i++) {
		((PFNSENDMESSAGEA)pfnPrevSendMessageW)(hListBox, LB_GETTEXT, i, (LPARAM)sz);
		OutputDebugStream(sz);
		original_list.push_back(tstring(sz));
	}

	auto addition_list = AutoCompleteHelpTip::OnQueryListBoxCustomAdded(hListBox, original_list);
	std::reverse(addition_list.begin(), addition_list.end());
	// ������]�������̂ŁA�R�s�[�����Ȃ�
	for (tstring& curnew : addition_list) {
		int exist = false;

		for (tstring& curorg : original_list) {
			if (curnew == curorg) {
				exist = true;
				break;
			}
		}

		// �����ꍇ��������
		if (!exist) {
			((PFNSENDMESSAGEA)pfnPrevSendMessageW)(hListBox, LB_INSERTSTRING, 1, (LPARAM)curnew.data());
			// ((PFNSENDMESSAGEA)pfnPrevSendMessageW)(hListBox, LB_INSERTSTRING, 0, (LPARAM)curnew.data());
		}

	}

	return TRUE;
}


