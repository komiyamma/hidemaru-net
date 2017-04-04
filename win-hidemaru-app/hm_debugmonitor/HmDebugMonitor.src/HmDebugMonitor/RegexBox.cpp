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
	GetClientRect(hWnd, &rc); // クライアント領域のサイズ

	// 希望するクライアント領域のサイズを持つウィンドウサイズを計算
	iClientWidth = (rc.right - rc.left);
	iClientHeight = (rc.bottom - rc.top);

	hRtLib = LoadLibrary("RICHED32.DLL");
	hRgxEdit = CreateWindowEx(WS_EX_CLIENTEDGE,
		"RICHEDIT",
		"",
		WS_CHILD | WS_VISIBLE | WS_BORDER,
		iClientWidth-RegexBoxWidth-5, 10,
		RegexBoxWidth, 22, //とりあえず幅、高さ０のウィンドウを作る
		hWnd,
		(HMENU)1,
		hInst,
		NULL);

	// 書き換えられると、EM_UPDATE が送られるようにするため。
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
		MessageBox(NULL, "ライブラリ解放失敗", "Error", MB_OK);
	}
}

void MoveRegexBox(HWND hWnd) {
	int iClientWidth = 0;
	int iClientHeight = 0;

	RECT rc;
	GetClientRect(hWnd, &rc); // クライアント領域のサイズ

	// 希望するクライアント領域のサイズを持つウィンドウサイズを計算
	iClientWidth = (rc.right - rc.left);
	iClientHeight = (rc.bottom - rc.top);

	// 移動
	MoveWindow(hRgxEdit, iClientWidth-RegexBoxWidth-5, 10, RegexBoxWidth, 22, FALSE);

	InvalidateRect(hRgxEdit, NULL, true);
}


static DWORD dwMyMask; //CHARFORMATで使うマスク値


static BOOL SetInitialFont(HWND hRgxEdit)//最初のフォントの設定
{
	CHARFORMAT cfm;
	memset(&cfm, 0, sizeof(CHARFORMAT));
	cfm.cbSize = sizeof(CHARFORMAT);
	cfm.dwMask = CFM_BOLD | CFM_CHARSET | CFM_COLOR |
		CFM_FACE | CFM_ITALIC | CFM_SIZE | CFM_STRIKEOUT |
		CFM_UNDERLINE;
	dwMyMask = cfm.dwMask;//以後dwMyMaskの値を使う
	cfm.bCharSet = SHIFTJIS_CHARSET;
	cfm.yHeight = 10*20; // 10 ポイント
	cfm.crTextColor = (COLORREF)0xffffff;
	strcpy(cfm.szFaceName, "ＭＳ ゴシック");
	if (SendMessage(hRgxEdit, EM_SETCHARFORMAT, SCF_SELECTION | SCF_WORD, (LPARAM)&cfm) == 0) {
		MessageBox(hRgxEdit, "EM_SETCHARFORMAT失敗です", "Error", MB_OK);
		return FALSE;
	}
	return TRUE; 
}



