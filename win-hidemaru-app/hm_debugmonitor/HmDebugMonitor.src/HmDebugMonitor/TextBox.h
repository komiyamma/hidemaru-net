/*
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the MIT License
 */

#pragma once

#include <windows.h>
#include <richedit.h>
#include <vector>

using namespace std;

const int TextBoxPosX = 5;
const int TextBoxPosY = 40;
const int TextBoxBottomPadding = 0; // HidemaruConsoleLineBox���������� 36;

const int TextBoxLimitSize = 1024 * 1024 * 10; // 10M


extern HWND hEdit;

HWND CreateTextBox(HWND hWnd, HINSTANCE hInst);
void DeleteTextBox();

BOOL SetInitialFont(HWND hWnd);
BOOL SetForegroundFont(HWND);
BOOL ChangeBackColor( HWND hWnd, COLORREF clr );
BOOL WriteRichEdit(HWND hWnd, char* pszBuf);
void MoveTextBox(HWND hWnd);

BOOL ReWriteAllRichEdit(HWND hWnd); // �e�L�X�g�{�b�N�X��AllRecievedLog�ɏ]���ď�������
BOOL ClearRichEdit(HWND hWnd);      // �e�L�X�g�{�b�N�X���N���A


#define OVER_ALL_RECIEVED_LOG_SIZE	800
#define JUST_ALL_RECIEVED_LOG_SIZE	200

extern vector<string> AllRecievedLog; // �t�B���^�[�����Ŏ�M�����S�Ẵ��O(SJIS��)

