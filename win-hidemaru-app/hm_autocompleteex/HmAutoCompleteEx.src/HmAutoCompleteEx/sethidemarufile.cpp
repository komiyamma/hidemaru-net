/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <shlwapi.h>
#include "tstring.h"

#pragma comment(lib, "shlwapi.lib")

#include "outputdebugstream.h"
#include "autocompletewindow.h"
#include "hookwin32api.h"
#include "outlinerwindow.h"
#include "sethidemarufile.h"
#include "hidemaruexe_export.h"

using namespace std;


static HWND hWndHidemaru = NULL;
static TCHAR szCurrentFileFullPath[MAX_PATH * 2] = _T(""); // �G�ۂ�SetHidemaruHandle���Ă΂ꂽ���ɍŏ��ɃX���b�g�ɓ���t�@�C�����B�r���Ńt�@�C�������ς�邱�Ƃ�����̂ŁA�����܂ł��Ȃ������ꍇ�p�r

void SetCurrentFileName(HWND hWndHidemaru) {

	// �G��8.66�ȏ�ł���΁A�����ƌ��ݐi�s�`�Ńt�@�C������ǂ�������
	if (CHidemaruExeExport::hm_version > 8.66 && hWndHidemaru) {

		// ���݂̏G�ۃE�B���h�E�̃t�@�C�����𓾂�B
		LRESULT cwch = SendMessage(hWndHidemaru, WM_HIDEMARUINFO, HIDEMARUINFO_GETFILEFULLPATH, (LPARAM)szCurrentFileFullPath);
	}
}

TCHAR *GetCurrentFileName() {
	SetCurrentFileName(hWndHidemaru);
	return szCurrentFileFullPath;
}

intptr_t SetHidemaruHandle(HWND pWndHidemaru, TCHAR *szCurFileName) {
	hWndHidemaru = pWndHidemaru;
	HookWin32apiFuncs();

	OutputDebugStream(_T("���ς�����u�Ԃ̃t�@�C����%s\n"), szCurFileName);

	// 2�Ԗڂ̃p�����[�^�Ƃ��ăt�@�C���������Ă�Ȃ�A�������B8.66�ȑO�p
	if (szCurFileName) {
		_tcscpy(szCurrentFileFullPath, szCurFileName);
	}
	SetCurrentFileName(hWndHidemaru);

	return TRUE;
}

