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

// �G�ۂ̓��͕⊮�p�̃��X�g�{�b�N�X(�I�Ȃ��̊܂�)���o�����钼�O
BOOL OnHidemaruListBoxAppear(HWND hWnd, WPARAM wparam) {
	// ����̃��X�g�{�b�N�X���̃C���f�b�N�X���w�������Ă����l�����Z�b�g
	ResetAutoCompleteListBoxIndex();
	OutputDebugStream("���X�g�{�b�N�X�̏o��");

	// �����⊮���̃w���v�`�b�v�̏���
	AutoCompleteHelpTip::OnCreate(hWnd, GetCurrentFileName());


	return 0;
}

BOOL OnHidemaruListBoxKeyDown(HWND hWnd, WPARAM wParam) {
	if (hWndAutoCompleteListBox) {
		OutputDebugStream("���X�g�{�b�N�X�̍X�V");
		int iGetCurSel = ((PFNSENDMESSAGEA)pfnPrevSendMessageA)(hWndAutoCompleteListBox, LB_GETCURSEL, 0, 0);

		OutputDebugStream("���ڐ� = %d\n���ڕ����� = %d\n",
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
		OutputDebugStream("���� PROC�� %d", wParam);
	}

	return CallWindowProc(
		pOriginalWIndowProcAutoCompleteListBox,  // ���̃E�B���h�E�v���V�[�W��
		hWnd,              // �E�B���h�E�̃n���h��
		uMsg,               // ���b�Z�[�W
		wParam,          // ���b�Z�[�W�̍ŏ��̃p�����[�^
		lParam           // ���b�Z�[�W�� 2 �Ԗڂ̃p�����[�^
	);
}


BOOL OnHidemaruListBoxAddComplete(HWND hListBox, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	OutputDebugStream("���ǉ����I������^��\n");

	int len = ((PFNSENDMESSAGEA)pfnPrevSendMessageA)(hListBox, LB_GETCOUNT, 0, 0);
	char sz[4096] = ""; // �G�ۂ̈�s�̍ő啶����
	vector<string> original_list;
	for (int i = 0; i < len; i++) {
		((PFNSENDMESSAGEA)pfnPrevSendMessageA)(hListBox, LB_GETTEXT, i, (LPARAM)sz);
		if (i == 0) {
			// ���s+�X�y�[�X�݂����Ȃ̂�����
			Matches m;
			if (OnigMatch(sz, "^(.+)   \\(.+?=.+?\\)$", &m)) {
				if (m[1].size() > 0) {
					strcpy(sz, m[1].data()); // m[1]�͕���������Ȃ̂ŁA��΃o�b�t�@�ɗ]�T������B
				}
			}

		}
		original_list.push_back(string(sz));
	}

	auto addition_list = AutoCompleteHelpTip::OnQueryListBoxCustomAdded(hListBox, original_list, GetCurrentFileName());
	std::reverse(addition_list.begin(), addition_list.end());

	// ������]�������̂ŁA�R�s�[�����Ȃ�
	for (string& curnew : addition_list) {
		int exist = false;

		for (string& curorg : original_list) {
			if (curnew == curorg) {
				exist = true;
				break;
			}
		}

		// �����ꍇ��������
		if (!exist) {
			((PFNSENDMESSAGEA)pfnPrevSendMessageA)(hListBox, LB_INSERTSTRING, 1, (LPARAM)curnew.data());
			// ((PFNSENDMESSAGEA)pfnPrevSendMessageA)(hListBox, LB_INSERTSTRING, 0, (LPARAM)curnew.data());
		}

	}

	return TRUE;
}


