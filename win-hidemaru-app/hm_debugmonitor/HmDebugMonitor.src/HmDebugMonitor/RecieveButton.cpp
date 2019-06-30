/*
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the MIT License
 */

#include "RecieveButton.h"
#include "TextBox.h"
#include "DebugMonitor.h"

HWND hRecieveButton = NULL;

static HFONT hFont = NULL;

HWND CreateRecieveButton(HWND hWnd, HINSTANCE hInst ) {

	 hRecieveButton = CreateWindow(
		"BUTTON",								 // ウィンドウクラス名
		"受信停止",								 // キャプション
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,   // スタイル指定
		TextBoxPosX, 10,                                  // 座標
		80, 21,                                  // サイズ
		hWnd,                                    // 親ウィンドウのハンドル
		(HMENU)RECIEVEBUTTON_ID,                 // メニューハンドル
		hInst,                                   // インスタンスハンドル
		NULL                                     // その他の作成データ
	);

	hFont = SetButtonFont("ＭＳ ゴシック", 14, 0);
	SendMessage(hRecieveButton, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
	return hRecieveButton;
}

void DeleteRecieveButton() {
	DeleteObject(hFont);
	DestroyWindow(hRecieveButton);
}

HFONT SetButtonFont(LPCTSTR face, int h, int angle) {
    HFONT hFont;
    hFont = CreateFont(h,    //フォント高さ
        0,                    //文字幅
        angle,                    //テキストの角度
        0,                    //ベースラインとｘ軸との角度
        FW_REGULAR,            //フォントの重さ（太さ）
        FALSE,                //イタリック体
        FALSE,                //アンダーライン
        FALSE,                //打ち消し線
        SHIFTJIS_CHARSET,    //文字セット
        OUT_DEFAULT_PRECIS,    //出力精度
        CLIP_DEFAULT_PRECIS,//クリッピング精度
        PROOF_QUALITY,        //出力品質
        FIXED_PITCH | FF_MODERN,//ピッチとファミリー
        face);    //書体名
    return hFont;
}


void SwitchRecieveStatus() {
	if ( isDoingRecieveFlag ) {
		isDoingRecieveFlag = FALSE;
		SetWindowText( hRecieveButton, "受信再開");
} else {
		iHidemaruErrorCnt = 0; // エラーカウントをリセット
		isDoingRecieveFlag = TRUE;
		SetWindowText( hRecieveButton, "受信停止");
	}
}