/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

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




// �t�b�N����֐��̃v���g�^�C�v���`
using PFNSENDMESSAGEA = LRESULT(WINAPI *)(HWND, UINT, WPARAM, LPARAM);

PROC pfnPrevSendMessageA = NULL; // �P�O��SendMessage�֐��B

int Lock_SendMessageA = FALSE;

// ����SendMessageA�ƑS�������`
LRESULT WINAPI Hook_SendMessageA(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!Lock_SendMessageA) {
		Lock_SendMessageA = TRUE;

		//-------------------------------------------- �������牺�̓I���W�i�����Ăяo������
		// �����R�[�h�̓���
		if (uMsg == 0x61E) {
			OutputDebugStream(_T("������SendmessageA %x, %x, %x"), hWnd, wParam, lParam);
			OnHidemaruApplicationKeyDown(hWnd, wParam, lParam);
			wLastKeyDown = wParam;
		}
		/*
		if (hWnd == hWndAutoCompleteListBox) {
		if (Msg == LB_GETTEXT) {
		}
		}
		else if (hWnd == hWndAutoCompleteListBox) {
		OutputDebugStream("�� %x, %x, %x\n", Msg, wParam, lParam);
		}
		*/

	}
	Lock_SendMessageA = FALSE;
	return ((PFNSENDMESSAGEA)pfnPrevSendMessageA)(hWnd, uMsg, wParam, lParam);
}

map<int, tstring> stringmap;

// �t�b�N����֐��̃v���g�^�C�v���`
using PFNSENDMESSAGEW = LRESULT(WINAPI *)(HWND, UINT, WPARAM, LPARAM);

PROC pfnPrevSendMessageW = NULL; // �P�O��SendMessage�֐��B
int Lock_SendMessageW = FALSE;

// ����SendMessageA�ƑS�������`
LRESULT WINAPI Hook_SendMessageW(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!Lock_SendMessageW) {
		Lock_SendMessageW = TRUE;

		//-------------------------------------------- �������牺�̓I���W�i�����Ăяo������
		// �����R�[�h�̓���
		if (uMsg == 0x61E) {
			OutputDebugStream(_T("������SendmessageW %x, %x, %x"), hWnd, wParam, lParam);
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
			OutputDebugStream("�������͕⊮����L�[�{�[�h�t�H�[�J�X��������");
			}
			*/
			// �ǉ����n�܂���
			if (uMsg == LB_INSERTSTRING || uMsg == LB_ADDSTRING) {
				if (bAutoCompleteListBoxAddMode == 0) {
					OnHidemaruListBoxAddCompletePrev(hWndAutoCompleteListBox, uMsg, wParam, lParam);
				}
				bAutoCompleteListBoxAddMode = 1;
			}
			// �ǉ����̃t���O�������Ă���̂ɁA�ǉ����߂����Ă��Ȃ�
			else if (bAutoCompleteListBoxAddMode == 1) {
				OnHidemaruListBoxAddCompletePost(hWndAutoCompleteListBox, uMsg, wParam, lParam);
				bAutoCompleteListBoxAddMode = 2;
			}
			if (uMsg == LB_GETTEXT && bAutoCompleteListBoxAddMode > 1 || uMsg == LB_GETTEXTLEN && bAutoCompleteListBoxAddMode && wParam == 0) {
				LRESULT cursor_index = ((PFNSENDMESSAGEA)pfnPrevSendMessageA)(hWnd, LB_GETCURSEL, 0, 0);
				LRESULT carret_index = ((PFNSENDMESSAGEA)pfnPrevSendMessageA)(hWnd, LB_GETCARETINDEX, 0, 0);

				// �I�΂�Ă��Ȃ�
				if (cursor_index == -1) {
					/*
					// wParam��0���傫�Ȓl�Ȃ�A�I��łȂ��̂ł͂Ȃ��A�Ō�̍��ڂ�I�����Ă��邩������Ȃ�
					if (wParam > 0) {
					// ���𒲂ׂ�
					int allcount = ((PFNSENDMESSAGEA)pfnPrevSendMessageA)(hWnd, LB_GETCOUNT, 0, 0);
					// wParam�ƌ�-1�������Ȃ�A�Ō�̒l��I�����Ă���
					if (wParam == allcount - 1) {
					// �Ō�̒l���w�����Ă��邱�ƂƏC������B
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
					OutputDebugStream(_T("�w���v�`�b�v�̃^�C�~���O!!%x, %x, %x, �J�[�\��%d, �J���b�g%d\n"), uMsg, wParam, lParam, cursor_index, carret_index);

					RECT rect;
					GetWindowRect(hWnd, &rect);  // �E�B���h�E�̍��W�l
					OutputDebugStream(_T("�����������W%d,%d,%d,%d\n"), rect.left, rect.top, rect.bottom, rect.right);

					LRESULT iItemHeight = ((PFNSENDMESSAGEA)pfnPrevSendMessageA)(hWnd, LB_GETITEMHEIGHT, 0, 0);

					if (cursor_index == 0) {
						// ���s+�X�y�[�X�݂����Ȃ̂�����
						tcmatch m;
						tregex re(AUTOCOMPLETE_HIDEMARU_ESPECIAL_ADD_WORD_REGEX);
						tstring result = regex_replace(szBuffer, re, _T(""));
						_tcscpy(szBuffer, result.data());
					}

					AutoCompleteHelpTip::OnListBoxSelectedIndexChanged(hWnd, (int)cursor_index, szBuffer, rect, (int)iItemHeight);
				}
			}

			else if (stringmap[uMsg].size() > 1) {
				OutputDebugStream(_T("�R�}���h ��%s\n"), stringmap[uMsg].data());
				OutputDebugStream(_T("�R�}���h %x, %x, %x\n"), uMsg, wParam, lParam);
			}
			else {
				OutputDebugStream(_T("�� ��%x\n"), uMsg);
				OutputDebugStream(_T("�� %x, %x, %x\n"), uMsg, wParam, lParam);
			}
		}
		else if (hWnd == hWndAutoCompleteListBox) {
			OutputDebugStream(_T("�� %x, %x, %x\n"), uMsg, wParam, lParam);
		}
	}

	Lock_SendMessageW = FALSE;
	return ((PFNSENDMESSAGEW)pfnPrevSendMessageW)(hWnd, uMsg, wParam, lParam);
}
//-----------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------


// �t�b�N����֐��̃v���g�^�C�v���`
using PFNCREATEWINDOWEXA = HWND (WINAPI *)(DWORD dwExStyle,      // �g���E�B���h�E�X�^�C��
	LPCTSTR lpClassName,  // �o�^����Ă���N���X��
	LPCTSTR lpWindowName, // �E�B���h�E��
	DWORD dwStyle,        // �E�B���h�E�X�^�C��
	int x,                // �E�B���h�E�̉������̈ʒu
	int y,                // �E�B���h�E�̏c�����̈ʒu
	int nWidth,           // �E�B���h�E�̕�
	int nHeight,          // �E�B���h�E�̍���
	HWND hWndParent,      // �e�E�B���h�E�܂��̓I�[�i�[�E�B���h�E�̃n���h��
	HMENU hMenu,          // ���j���[�n���h���܂��͎q���ʎq
	HINSTANCE hInstance,  // �A�v���P�[�V�����C���X�^���X�̃n���h��
	LPVOID lpParam        // �E�B���h�E�쐬�f�[�^
);

PROC pfnPrevCreateWindowExA = NULL; // �P�O��SendMessage�֐��B

int Lock_CreateWindowExA = FALSE;

// ����SendMessageA�ƑS�������`
HWND WINAPI Hook_CreateWindowExA(DWORD dwExStyle,      // �g���E�B���h�E�X�^�C��
	LPCTSTR lpClassName,  // �o�^����Ă���N���X��
	LPCTSTR lpWindowName, // �E�B���h�E��
	DWORD dwStyle,        // �E�B���h�E�X�^�C��
	int x,                // �E�B���h�E�̉������̈ʒu
	int y,                // �E�B���h�E�̏c�����̈ʒu
	int nWidth,           // �E�B���h�E�̕�
	int nHeight,          // �E�B���h�E�̍���
	HWND hWndParent,      // �e�E�B���h�E�܂��̓I�[�i�[�E�B���h�E�̃n���h��
	HMENU hMenu,          // ���j���[�n���h���܂��͎q���ʎq
	HINSTANCE hInstance,  // �A�v���P�[�V�����C���X�^���X�̃n���h��
	LPVOID lpParam        // �E�B���h�E�쐬�f�[�^
	)
{
	//-------------------------------------------- �������牺�̓I���W�i�����Ăяo������
	HWND ret = ((PFNCREATEWINDOWEXA)pfnPrevCreateWindowExA)(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

	if (!Lock_CreateWindowExA) {
		Lock_CreateWindowExA = TRUE;

		HWND parent = GetParent(ret);
		if (parent) {
			TCHAR sz[100];
			GetClassName(parent, sz, sizeof(sz));
			if (_tcscmp(sz, _T("HidemaruAutocomp")) == 0) {
				hWndAutoCompleteListBox = ret;
				OutputDebugStream(_T("��A�̕�\n"));
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


// �t�b�N����֐��̃v���g�^�C�v���`
using PFNCREATEWINDOWEXW = HWND(WINAPI *)(
	DWORD dwExStyle,      // �g���E�B���h�E�X�^�C��
	LPCTSTR lpClassName,  // �o�^����Ă���N���X��
	LPCTSTR lpWindowName, // �E�B���h�E��
	DWORD dwStyle,        // �E�B���h�E�X�^�C��
	int x,                // �E�B���h�E�̉������̈ʒu
	int y,                // �E�B���h�E�̏c�����̈ʒu
	int nWidth,           // �E�B���h�E�̕�
	int nHeight,          // �E�B���h�E�̍���
	HWND hWndParent,      // �e�E�B���h�E�܂��̓I�[�i�[�E�B���h�E�̃n���h��
	HMENU hMenu,          // ���j���[�n���h���܂��͎q���ʎq
	HINSTANCE hInstance,  // �A�v���P�[�V�����C���X�^���X�̃n���h��
	LPVOID lpParam        // �E�B���h�E�쐬�f�[�^
	);

PROC pfnPrevCreateWindowExW = NULL; // �P�O��SendMessage�֐��B

int Lock_CreateWindowExW = FALSE;

HWND WINAPI Hook_CreateWindowExW(DWORD dwExStyle,      // �g���E�B���h�E�X�^�C��
	LPCTSTR lpClassName,  // �o�^����Ă���N���X��
	LPCTSTR lpWindowName, // �E�B���h�E��
	DWORD dwStyle,        // �E�B���h�E�X�^�C��
	int x,                // �E�B���h�E�̉������̈ʒu
	int y,                // �E�B���h�E�̏c�����̈ʒu
	int nWidth,           // �E�B���h�E�̕�
	int nHeight,          // �E�B���h�E�̍���
	HWND hWndParent,      // �e�E�B���h�E�܂��̓I�[�i�[�E�B���h�E�̃n���h��
	HMENU hMenu,          // ���j���[�n���h���܂��͎q���ʎq
	HINSTANCE hInstance,  // �A�v���P�[�V�����C���X�^���X�̃n���h��
	LPVOID lpParam        // �E�B���h�E�쐬�f�[�^
	)
{
	//-------------------------------------------- �������牺�̓I���W�i�����Ăяo������
	HWND ret = ((PFNCREATEWINDOWEXA)pfnPrevCreateWindowExW)(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
	if (!Lock_CreateWindowExW) {
		Lock_CreateWindowExW = TRUE;

		HWND parent = GetParent(ret);
		if (parent) {
			TCHAR sz[100];
			GetClassName(parent, sz, sizeof(sz));
			if (_tcscmp(sz, _T("HidemaruAutocomp")) == 0) {
				hWndAutoCompleteListBox = ret;
				OutputDebugStream(_T("��W�̕�\n"));
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



// �t�b�N����֐��̃v���g�^�C�v���`
using PFNDESTROYWINDOW = BOOL(WINAPI *)(
	HWND hWnd  // �j������E�B���h�E�̃n���h��
);

PROC pfnPrevDestroyWindow = NULL; // �P�O��DestroyWindow�֐��B

int Lock_DestroyWindow = FALSE;

// ����SendMessageA�ƑS�������`
BOOL WINAPI Hook_DestroyWindow(HWND hWnd)  // �j������E�B���h�E�̃n���h��
	
{
	if (!Lock_DestroyWindow) {
		Lock_DestroyWindow = TRUE;

		TCHAR cname[100];
		GetClassName(hWnd, cname, sizeof(cname));
		if (hWnd == hWndAutoCompleteListBox || _tcscmp(cname, _T("HidemaruAutocomp") ) == 0) {

			// �����⊮���̃w���v�`�b�v�̏���
			AutoCompleteHelpTip::OnDestroy(hWndAutoCompleteListBox);

			hWndAutoCompleteListBox = NULL;
			ResetAutoCompleteListBoxIndex();
			OutputDebugStream(_T("����DestroyAutoCompleteWindow\n"));
		}
	}

	Lock_DestroyWindow = FALSE;
	//-------------------------------------------- �������牺�̓I���W�i�����Ăяo������
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