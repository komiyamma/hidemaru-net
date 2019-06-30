/*
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the MIT License
 */

#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <process.h>
#include <commctrl.h>
#include "resource.h"
#include "DebugMonitor.h"
#include "TextBox.h"
#include "RegexBox.h"
#include "TextLabel.h"
#include "RecieveButton.h"
#include "ClearButton.h"


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL InitApp(HINSTANCE);
BOOL InitInstance(HINSTANCE, int);


char szClassName[] = "HM_DebugMonitor";        //�E�B���h�E�N���X
HINSTANCE hInst;

int WINAPI WinMain(HINSTANCE hCurInst, HINSTANCE hPrevInst,  LPSTR lpsCmdLine, int nCmdShow) {

	// -----------���łɑ��݂���ꍇ�̏���
	// �E�B���h�E�^�C�v�̏ꍇ���l���āA�A�C�R��������Ă�����A���ւƖ߂��B
	HWND hWndDebugMonitor = FindWindow(szClassName, NULL);

	// ���݂��Ă�����
	if ( hWndDebugMonitor ) {

		// �A�C�R��������ĂĂ����ւƖ߂��B
		OpenIcon( hWndDebugMonitor );

		return FALSE;
	}

	// -----------�ȉ��V�K�N���̏���
	MSG msg;

	if (!InitApp(hCurInst))
		return FALSE;
	if (!InitInstance(hCurInst, nCmdShow)) 
		return FALSE;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}



//�E�B���h�E�E�N���X�̓o�^

BOOL InitApp(HINSTANCE hInst) {
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;    //�v���V�[�W����
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;        //�C���X�^���X
	wc.hIcon =  LoadIcon(hInst , MAKEINTRESOURCE(IDI_ICON));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	// wc.lpszMenuName = "IDR_MENU1";    //���j���[��
	wc.lpszClassName = (LPCSTR)szClassName;
	//wc.hIconSm = wc.hIcon;
	wc.hIconSm = (HICON)LoadImage( hInst, MAKEINTRESOURCE(IDI_ICON), IMAGE_ICON, 16, 16,  LR_DEFAULTCOLOR );
	return (RegisterClassEx(&wc));
}


//�E�B���h�E�̐���

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
	HWND hWnd;
	hInst = hInstance;

	hWnd = CreateWindow(szClassName,
		"�G�ۃG�f�B�^�E�f�o�b�O�o�̓��j�^�[",    //�^�C�g���o�[�ɂ��̖��O���\������܂�
		WS_OVERLAPPEDWINDOW ,//�E�B���h�E�̎�� �T�C�Y�Œ�
		CW_USEDEFAULT,    //�w���W
		CW_USEDEFAULT,    //�x���W
		500,    //��
		480,    //����
		NULL,//�e�E�B���h�E�̃n���h���A�e�����Ƃ���NULL
		NULL,//���j���[�n���h���A�N���X���j���[���g���Ƃ���NULL
		hInstance,//�C���X�^���X�n���h��
		NULL);
	if (!hWnd) {
		return FALSE;
	}
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return TRUE;
}



// �X���b�h�n���h��
HANDLE hTh;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg) {

	case WM_CREATE:

			CreateTextBox(hWnd, hInst);
			CreateRegexBox(hWnd, hInst);
			CreateRecieveButton(hWnd, hInst);
			CreateClearButton(hWnd, hInst);
			// CreateConsoleLineBox(hWnd, hInst);
			InitDebugMonitor(hEdit);

			//--------------------------------------------------
			// �X���b�h�쐬
			unsigned int thID;
			hTh = (HANDLE)_beginthreadex(NULL, 0, ThreadExternalProcDebugMonitor, NULL, 0, &thID);
			if (hTh == 0) {
				// cout << "�X���b�h�쐬���s" << endl;
				// return -1; // ���s�����玸�s�����܂ŁB�I��点�Ȃ��B
			}

			break;

		case WM_PAINT:
			CreateRegexBoxTextLabel(hWnd);
			MoveRegexBox(hWnd);
			MoveTextBox(hWnd);
			// MoveConsoleLineBox(hWnd);
			break;


		case WM_SIZE:

			// MoveWindow(hEdit, 0, 0, LOWORD(lp), HIWORD(lp), TRUE);
			break;

		case WM_NOTIFY:
			if (((LPNMHDR)lp)->code == EN_MSGFILTER) {
			}

			break;

		case WM_COMMAND:

			// ���K�\���{�b�N�X������������ꂽ����
			if (lp && (HWND)lp==hRgxEdit && HIWORD(wp) == EN_UPDATE) {
				GetWindowText(hRgxEdit , hRgxEditContent , sizeof(hRgxEditContent));
				ReWriteAllRichEdit( hEdit );
			}
			switch (LOWORD(wp)) {
				case IDM_END:
					SendMessage(hWnd, WM_CLOSE, 0, 0);
					break;
				case IDM_FONT:
					SetForegroundFont(hEdit);
					break;
				case RECIEVEBUTTON_ID:
					SwitchRecieveStatus();
					break;
				case CLEARBUTTON_ID:
					ClearRichEdit(hEdit);
					break;
			}
			break;

		case WM_CLOSE:
			// �X���b�h�̃N���[�Y
			if (hTh != NULL) {
				CloseHandle(hTh);
			}
			DeleteTextBox();
			DeleteRegexBox();
			DeleteRecieveButton();
			DeleteClearButton();
			DeleteRegexBoxTextLabel();
			// DeleteConsoleLineBox();
			DestroyWindow(hWnd);

			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			return (DefWindowProc(hWnd, msg, wp, lp));
	}
	return 0L;
}
