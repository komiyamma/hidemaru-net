/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#include <windows.h>
#include <tchar.h>
#include "outputdebugstream.h"

// ������
// ���X�g�{�b�N�X�́A�A�E�g���C�i�[�Ɏg����B
void AnalizeOutlinerListBox(HWND wnd) {

	OutputDebugStream(_T("�A�E�g���C�i %x"), wnd);
	TCHAR szTitle[1024];
	GetWindowText(wnd, szTitle, sizeof(szTitle));
	OutputDebugStream(_T("�A�E�g���C�i �^�C�g��%s"), szTitle);

	OutputDebugStream(_T("�A�E�g���C�i %x"), wnd);
	LRESULT lb_getcursel = SendMessageA(wnd, LB_GETCURSEL, 0, 0);
	OutputDebugStream(_T("���݂̃A�E�g���C�i���̃p�X��̃C���f�b�N�X%d"), lb_getcursel);
	LRESULT lb_getcount = SendMessageA(wnd, LB_GETCOUNT, 0, 0);
	OutputDebugStream(_T("���݂̃A�E�g���C�i���̃C���f�b�N�X����%d"), lb_getcount);
	LRESULT lb_gettextlen_of_cursel = SendMessageA(wnd, LB_GETTEXTLEN, lb_getcursel, 0);
	OutputDebugStream(_T("���݂̃A�E�g���C�i���̌��݂̃C���f�b�N�X���̕�����%d"), lb_gettextlen_of_cursel);
}
