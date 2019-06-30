/*
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the MIT License
 */

#include "ClearButton.h"
#include "TextBox.h"
#include "DebugMonitor.h"

HWND hClearButton = NULL;

static HFONT hFont = NULL;

HWND CreateClearButton(HWND hWnd, HINSTANCE hInst ) {

	 hClearButton = CreateWindow(
		"BUTTON",								 // �E�B���h�E�N���X��
		"�N���A",								 // �L���v�V����
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,   // �X�^�C���w��
		ClearButtonPosX, 10,                     // ���W
		50, 21,                                  // �T�C�Y
		hWnd,                                    // �e�E�B���h�E�̃n���h��
		(HMENU)CLEARBUTTON_ID,                   // ���j���[�n���h��
		hInst,                                   // �C���X�^���X�n���h��
		NULL                                     // ���̑��̍쐬�f�[�^
	);

	hFont = SetButtonFont("�l�r �S�V�b�N", 14, 0);
	SendMessage(hClearButton, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
	return hClearButton;
}

void DeleteClearButton() {
	DeleteObject(hFont);
	DestroyWindow(hClearButton);
}


