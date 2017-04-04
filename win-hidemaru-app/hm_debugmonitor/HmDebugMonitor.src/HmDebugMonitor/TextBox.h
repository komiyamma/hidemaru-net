#pragma once

#include <windows.h>
#include <richedit.h>
#include <vector>

using namespace std;

const int TextBoxPosX = 5;
const int TextBoxPosY = 40;
const int TextBoxBottomPadding = 0; // HidemaruConsoleLineBoxがあったら 36;

const int TextBoxLimitSize = 1024 * 1024 * 10; // 10M


extern HWND hEdit;

HWND CreateTextBox(HWND hWnd, HINSTANCE hInst);
void DeleteTextBox();

BOOL SetInitialFont(HWND hWnd);
BOOL SetForegroundFont(HWND);
BOOL ChangeBackColor( HWND hWnd, COLORREF clr );
BOOL WriteRichEdit(HWND hWnd, char* pszBuf);
void MoveTextBox(HWND hWnd);

BOOL ReWriteAllRichEdit(HWND hWnd); // テキストボックスをAllRecievedLogに従って書き換え
BOOL ClearRichEdit(HWND hWnd);      // テキストボックスをクリア


#define OVER_ALL_RECIEVED_LOG_SIZE	800
#define JUST_ALL_RECIEVED_LOG_SIZE	200

extern vector<string> AllRecievedLog; // フィルター無しで受信した全てのログ(SJIS済)

