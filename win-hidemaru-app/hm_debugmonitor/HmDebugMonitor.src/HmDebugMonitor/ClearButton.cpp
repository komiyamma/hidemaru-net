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
		"BUTTON",								 // ウィンドウクラス名
		"クリア",								 // キャプション
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,   // スタイル指定
		ClearButtonPosX, 10,                     // 座標
		50, 21,                                  // サイズ
		hWnd,                                    // 親ウィンドウのハンドル
		(HMENU)CLEARBUTTON_ID,                   // メニューハンドル
		hInst,                                   // インスタンスハンドル
		NULL                                     // その他の作成データ
	);

	hFont = SetButtonFont("ＭＳ ゴシック", 14, 0);
	SendMessage(hClearButton, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
	return hClearButton;
}

void DeleteClearButton() {
	DeleteObject(hFont);
	DestroyWindow(hClearButton);
}


