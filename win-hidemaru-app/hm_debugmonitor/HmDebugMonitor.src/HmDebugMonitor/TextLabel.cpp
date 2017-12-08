/*
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the MIT License
 */

#include "TextLabel.h"
#include "RegexBox.h"

static HFONT hFont = NULL;

// 正規表現受信フィルター
static char *str = "絞込《正規表現》→"; 
void CreateRegexBoxTextLabel(HWND hWnd) {

	int iClientWidth = 0;
	int iClientHeight = 0;

	RECT rc;
	GetClientRect(hWnd, &rc); // クライアント領域のサイズ

	// 希望するクライアント領域のサイズを持つウィンドウサイズを計算
	iClientWidth = (rc.right - rc.left);
	iClientHeight = (rc.bottom - rc.top);


	HDC hdc;
    PAINTSTRUCT paint;

    hdc = BeginPaint(hWnd, &paint);
	hFont = SetLabelFont("ＭＳ ゴシック", 14, 0);
	SelectObject( hdc, hFont );
	SetTextColor(hdc, RGB(30, 0, 0));

    TextOut(hdc, iClientWidth-RegexBoxWidth-5-130, 13, (LPCSTR)str, strlen(str));
    EndPaint(hWnd, &paint);
}

void DeleteRegexBoxTextLabel() {
	DeleteObject(hFont);
}

HFONT SetLabelFont(LPCTSTR face, int h, int angle) {
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