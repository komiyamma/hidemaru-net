/*
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the MIT License
 */

#define _CRT_SECURE_NO_WARNINGS

#include "TextBox.h"
#include "RegexBox.h"
#include "OnigRegex.h"
#include <vector>

using namespace std;

static HINSTANCE hRtLib;
HWND hEdit;
vector<string> AllRecievedLog; // フィルター無しで受信した全てのログ(SJIS済)

HWND CreateTextBox(HWND hWnd, HINSTANCE hInst ) {

	int iClientWidth = 0;
	int iClientHeight = 0;

	RECT rc;
	GetClientRect(hWnd, &rc); // クライアント領域のサイズ

	// 希望するクライアント領域のサイズを持つウィンドウサイズを計算
	iClientWidth = (rc.right - rc.left);
	iClientHeight = (rc.bottom - rc.top);

	hRtLib = LoadLibrary("RICHED32.DLL");
	hEdit = CreateWindowEx(WS_EX_CLIENTEDGE,
		"RICHEDIT",
		"",
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE |
		WS_VSCROLL | WS_HSCROLL | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
		TextBoxPosX, TextBoxPosY,
		iClientWidth - TextBoxPosX * 2, iClientHeight - TextBoxPosY - TextBoxPosX - TextBoxBottomPadding, //とりあえず幅、高さ０のウィンドウを作る
		hWnd,
		(HMENU)1,
		hInst,
		NULL);
	ChangeBackColor( hEdit, 0x330000 );
	SetInitialFont(hEdit);

	//リッチエディットコントロールの最大文字数のセット
    SendMessage(hEdit, EM_EXLIMITTEXT, 0, (LPARAM)TextBoxLimitSize);

	return hEdit;
}

void DeleteTextBox() {
	DestroyWindow(hEdit);
	if(FreeLibrary(hRtLib) == 0) {
		MessageBox(NULL, "ライブラリ解放失敗", "Error", MB_OK);
	}
}

void MoveTextBox(HWND hWnd) {
	int iClientWidth = 0;
	int iClientHeight = 0;

	RECT rc;
	GetClientRect(hWnd, &rc); // クライアント領域のサイズ

	// 希望するクライアント領域のサイズを持つウィンドウサイズを計算
	iClientWidth = (rc.right - rc.left);
	iClientHeight = (rc.bottom - rc.top);

	// 移動
	MoveWindow(hEdit, TextBoxPosX, TextBoxPosY, iClientWidth - TextBoxPosX * 2, iClientHeight - TextBoxPosY - TextBoxPosX - TextBoxBottomPadding, FALSE);
}

static DWORD dwMyMask; //CHARFORMATで使うマスク値


static BOOL SetInitialFont(HWND hEdit)//最初のフォントの設定
{
	CHARFORMAT cfm;
	memset(&cfm, 0, sizeof(CHARFORMAT));
	cfm.cbSize = sizeof(CHARFORMAT);
	cfm.dwMask = CFM_BOLD | CFM_CHARSET | CFM_COLOR |
		CFM_FACE | CFM_ITALIC | CFM_SIZE | CFM_STRIKEOUT |
		CFM_UNDERLINE;
	dwMyMask = cfm.dwMask;//以後dwMyMaskの値を使う
	cfm.bCharSet = SHIFTJIS_CHARSET;
	cfm.yHeight = 9*20; // 10 ポイント
	cfm.crTextColor = (COLORREF)0xffffff;
	strcpy(cfm.szFaceName, "ＭＳ ゴシック");
	if (SendMessage(hEdit, EM_SETCHARFORMAT, SCF_SELECTION | SCF_WORD, (LPARAM)&cfm) == 0) {
		MessageBox(hEdit, "EM_SETCHARFORMAT失敗です", "Error", MB_OK);
		return FALSE;
	}
	return TRUE; 
}

BOOL SetForegroundFont(HWND hEdit)
{
	CHARFORMAT cfm;
	CHOOSEFONT cf;
	LOGFONT lf;
	HDC hDC;

	memset(&cf, 0, sizeof(CHOOSEFONT));
	memset(&lf, 0, sizeof(LOGFONT));
	cfm.cbSize = sizeof(CHARFORMAT);


	//今までの設定を取得してそれをCHOOSEFONT構造体に渡す
	SendMessage(hEdit, EM_GETCHARFORMAT, TRUE, (LPARAM)&cfm);
	hDC = GetDC(hEdit);
	lf.lfHeight = MulDiv(cfm.yHeight,
		GetDeviceCaps(hDC, LOGPIXELSY), -1440);
	ReleaseDC(hEdit, hDC);
	cfm.dwMask = dwMyMask;
	if (cfm.dwEffects & CFE_BOLD)
		lf.lfWeight = FW_BOLD;
	else
		lf.lfWeight = FW_NORMAL;
	if (cfm.dwEffects & CFE_ITALIC)
		lf.lfItalic = TRUE;
	if (cfm.dwEffects & CFE_UNDERLINE)
		lf.lfUnderline = TRUE;
	if (cfm.dwEffects & CFE_STRIKEOUT)
		lf.lfStrikeOut = TRUE;
	lf.lfCharSet = cfm.bCharSet;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = cfm.bPitchAndFamily;
	strcpy( lf.lfFaceName, cfm.szFaceName );
	cf.rgbColors = cfm.crTextColor;
	cf.lStructSize = sizeof(CHOOSEFONT);
	cf.hwndOwner = hEdit;
	cf.lpLogFont = &lf;
	cf.Flags = CF_SCREENFONTS | CF_EFFECTS | CF_INITTOLOGFONTSTRUCT;

	//フォント選択ダイアログを出して新しい設定を取得する
	if (ChooseFont(&cf))
	{
		cfm.cbSize = sizeof(CHARFORMAT);
		cfm.dwMask = dwMyMask;
		cfm.yHeight = 2 * cf.iPointSize;
		cfm.dwEffects = 0;
		if (lf.lfWeight >= FW_BOLD)
			cfm.dwEffects |= CFE_BOLD;
		if (lf.lfItalic)
			cfm.dwEffects |= CFE_ITALIC;
		if (lf.lfUnderline)
			cfm.dwEffects |= CFE_UNDERLINE;
		if (lf.lfStrikeOut)
			cfm.dwEffects |= CFE_STRIKEOUT;
		cfm.crTextColor = (COLORREF)cf.rgbColors;
		cfm.bPitchAndFamily = lf.lfPitchAndFamily;
		cfm.bCharSet = lf.lfCharSet;
		strcpy(cfm.szFaceName, lf.lfFaceName);
		if(SendMessage(hEdit, EM_SETCHARFORMAT, 
			SCF_SELECTION, (LPARAM)&cfm) == 0) {
				MessageBox(hEdit, "失敗です", "Error", MB_OK);
				return FALSE;
		}
	}
	SetFocus(hEdit);
	return TRUE;
}


BOOL ChangeBackColor( HWND hEdit, COLORREF clr )
{
	SendMessage( hEdit, EM_SETBKGNDCOLOR, (WPARAM)0, (LPARAM)clr);

	return TRUE;
}


BOOL WriteRichEdit(HWND hEdit, char* pszBuf)
{
	//エディットすべての文字列の長さを取得
	int len = SendMessage(hEdit,WM_GETTEXTLENGTH,0,0);
	// 最後に移動
	SendMessage( hEdit, EM_SETSEL, len, len );

	// 最後までスクロール
	SendMessage( hEdit, EM_LINESCROLL, 0, AllRecievedLog.size() );

	// 何か正規表現に書き込んでいたら
	if ( hRgxEditContent && strlen(hRgxEditContent) > 0 ) {

		// それにマッチする場合だけEditBoxに書き込む
		if ( OnigMatch( pszBuf, hRgxEditContent ) ) {
			SendMessage( hEdit, EM_REPLACESEL, FALSE, (LPARAM)(LPCTSTR)pszBuf );
		}

	// 絞り込みが空なら全部書く
	} else {
		SendMessage( hEdit, EM_REPLACESEL, FALSE, (LPARAM)(LPCTSTR)pszBuf );
	}

	UpdateWindow(hEdit);
	return TRUE;
}


BOOL ReWriteAllRichEdit(HWND hEdit) {

	// 最初にカーソルを移動
    SendMessage( hEdit, EM_SETSEL,(WPARAM)0,(LPARAM)0 );
	SendMessage( hEdit, WM_SETTEXT, (WPARAM)0, (LPARAM)0 );

	// フォントを改めて設定
	SetInitialFont( hEdit );

	for each ( string line in AllRecievedLog) {
		// 正規表現のところ中身空なら全部だす。
		if ( strlen( hRgxEditContent ) == 0 ) {
			WriteRichEdit(hEdit, (char *)line.c_str() );

		// それ以外なら正規表現にマッチするもののみ。
		} else if ( OnigMatch( line.c_str(), hRgxEditContent ) ) {
			WriteRichEdit(hEdit, (char *)line.c_str() );
		}
	}

	// 最後までスクロール
	SendMessage( hEdit, EM_LINESCROLL, 0, AllRecievedLog.size() );
	UpdateWindow(hEdit);

	return TRUE;
}

BOOL ClearRichEdit(HWND hEdit) {

	// 最初にカーソルを移動
    SendMessage( hEdit, EM_SETSEL,(WPARAM)0,(LPARAM)0 );
	SendMessage( hEdit, WM_SETTEXT, (WPARAM)0, (LPARAM)0 );

	// フォントを改めて設定
	SetInitialFont( hEdit );

	AllRecievedLog.clear();
	UpdateWindow(hEdit);

	return TRUE;
}

