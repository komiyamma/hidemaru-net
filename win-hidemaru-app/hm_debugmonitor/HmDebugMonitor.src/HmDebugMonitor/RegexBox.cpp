/*
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the MIT License
 */

#define _CRT_SECURE_NO_WARNINGS

#include "TextBox.h"
#include "RegexBox.h"

static HINSTANCE hRtLib;
HWND hRgxEdit;

char hRgxEditContent[4096] = "";


HWND CreateRegexBox(HWND hWnd, HINSTANCE hInst ) {

	int iClientWidth = 0;
	int iClientHeight = 0;

	RECT rc;
	GetClientRect(hWnd, &rc); // �N���C�A���g�̈�̃T�C�Y

	// ��]����N���C�A���g�̈�̃T�C�Y�����E�B���h�E�T�C�Y���v�Z
	iClientWidth = (rc.right - rc.left);
	iClientHeight = (rc.bottom - rc.top);

	hRtLib = LoadLibrary("RICHED32.DLL");
	hRgxEdit = CreateWindowEx(WS_EX_CLIENTEDGE,
		"RICHEDIT",
		"",
		WS_CHILD | WS_VISIBLE | WS_BORDER,
		iClientWidth-RegexBoxWidth-5, 10,
		RegexBoxWidth, 22, //�Ƃ肠�������A�����O�̃E�B���h�E�����
		hWnd,
		(HMENU)1,
		hInst,
		NULL);

	// ������������ƁAEM_UPDATE ��������悤�ɂ��邽�߁B
    DWORD dwEvent = SendMessage(hRgxEdit, EM_GETEVENTMASK, 0, 0);
    dwEvent |= /*ENM_MOUSEEVENTS | ENM_SELCHANGE | */ ENM_UPDATE;
    SendMessage(hRgxEdit, EM_SETEVENTMASK, 0, (LPARAM)dwEvent);

	ChangeBackColor( hRgxEdit, 0x330000 );
	SetInitialFont(hRgxEdit);

	return hRgxEdit;
}

void DeleteRegexBox() {
	DestroyWindow(hRgxEdit);
	if(FreeLibrary(hRtLib) == 0) {
		MessageBox(NULL, "���C�u����������s", "Error", MB_OK);
	}
}

void MoveRegexBox(HWND hWnd) {
	int iClientWidth = 0;
	int iClientHeight = 0;

	RECT rc;
	GetClientRect(hWnd, &rc); // �N���C�A���g�̈�̃T�C�Y

	// ��]����N���C�A���g�̈�̃T�C�Y�����E�B���h�E�T�C�Y���v�Z
	iClientWidth = (rc.right - rc.left);
	iClientHeight = (rc.bottom - rc.top);

	// �ړ�
	MoveWindow(hRgxEdit, iClientWidth-RegexBoxWidth-5, 10, RegexBoxWidth, 22, FALSE);

	InvalidateRect(hRgxEdit, NULL, true);
}


static DWORD dwMyMask; //CHARFORMAT�Ŏg���}�X�N�l


static BOOL SetInitialFont(HWND hRgxEdit)//�ŏ��̃t�H���g�̐ݒ�
{
	CHARFORMAT cfm;
	memset(&cfm, 0, sizeof(CHARFORMAT));
	cfm.cbSize = sizeof(CHARFORMAT);
	cfm.dwMask = CFM_BOLD | CFM_CHARSET | CFM_COLOR |
		CFM_FACE | CFM_ITALIC | CFM_SIZE | CFM_STRIKEOUT |
		CFM_UNDERLINE;
	dwMyMask = cfm.dwMask;//�Ȍ�dwMyMask�̒l���g��
	cfm.bCharSet = SHIFTJIS_CHARSET;
	cfm.yHeight = 10*20; // 10 �|�C���g
	cfm.crTextColor = (COLORREF)0xffffff;
	strcpy(cfm.szFaceName, "�l�r �S�V�b�N");
	if (SendMessage(hRgxEdit, EM_SETCHARFORMAT, SCF_SELECTION | SCF_WORD, (LPARAM)&cfm) == 0) {
		MessageBox(hRgxEdit, "EM_SETCHARFORMAT���s�ł�", "Error", MB_OK);
		return FALSE;
	}
	return TRUE; 
}



