/*
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the MIT License
 */

#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <process.h>
#include <commctrl.h>
#include "resource.h"
#include "DebugMonitor.h"
#include "TextBox.h"
#include "RegexBox.h"
#include "TextLabel.h"
#include "RecieveButton.h"
#include "ClearButton.h"


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL InitApp(HINSTANCE);
BOOL InitInstance(HINSTANCE, int);


char szClassName[] = "HM_DebugMonitor";        //ウィンドウクラス
HINSTANCE hInst;

int WINAPI WinMain(HINSTANCE hCurInst, HINSTANCE hPrevInst,  LPSTR lpsCmdLine, int nCmdShow) {

	// -----------すでに存在する場合の処理
	// ウィンドウタイプの場合も考えて、アイコン化されていたら、元へと戻す。
	HWND hWndDebugMonitor = FindWindow(szClassName, NULL);

	// 存在していたら
	if ( hWndDebugMonitor ) {

		// アイコン化されてても元へと戻す。
		OpenIcon( hWndDebugMonitor );

		return FALSE;
	}

	// -----------以下新規起動の処理
	MSG msg;

	if (!InitApp(hCurInst))
		return FALSE;
	if (!InitInstance(hCurInst, nCmdShow)) 
		return FALSE;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}



//ウィンドウ・クラスの登録

BOOL InitApp(HINSTANCE hInst) {
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;    //プロシージャ名
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;        //インスタンス
	wc.hIcon =  LoadIcon(hInst , MAKEINTRESOURCE(IDI_ICON));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	// wc.lpszMenuName = "IDR_MENU1";    //メニュー名
	wc.lpszClassName = (LPCSTR)szClassName;
	//wc.hIconSm = wc.hIcon;
	wc.hIconSm = (HICON)LoadImage( hInst, MAKEINTRESOURCE(IDI_ICON), IMAGE_ICON, 16, 16,  LR_DEFAULTCOLOR );
	return (RegisterClassEx(&wc));
}


//ウィンドウの生成

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
	HWND hWnd;
	hInst = hInstance;

	hWnd = CreateWindow(szClassName,
		"秀丸エディタ・デバッグ出力モニター",    //タイトルバーにこの名前が表示されます
		WS_OVERLAPPEDWINDOW ,//ウィンドウの種類 サイズ固定
		CW_USEDEFAULT,    //Ｘ座標
		CW_USEDEFAULT,    //Ｙ座標
		500,    //幅
		480,    //高さ
		NULL,//親ウィンドウのハンドル、親を作るときはNULL
		NULL,//メニューハンドル、クラスメニューを使うときはNULL
		hInstance,//インスタンスハンドル
		NULL);
	if (!hWnd) {
		return FALSE;
	}
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return TRUE;
}



// スレッドハンドル
HANDLE hTh;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg) {

	case WM_CREATE:

			CreateTextBox(hWnd, hInst);
			CreateRegexBox(hWnd, hInst);
			CreateRecieveButton(hWnd, hInst);
			CreateClearButton(hWnd, hInst);
			// CreateConsoleLineBox(hWnd, hInst);
			InitDebugMonitor(hEdit);

			//--------------------------------------------------
			// スレッド作成
			unsigned int thID;
			hTh = (HANDLE)_beginthreadex(NULL, 0, ThreadExternalProcDebugMonitor, NULL, 0, &thID);
			if (hTh == 0) {
				// cout << "スレッド作成失敗" << endl;
				// return -1; // 失敗したら失敗したまで。終わらせない。
			}

			break;

		case WM_PAINT:
			CreateRegexBoxTextLabel(hWnd);
			MoveRegexBox(hWnd);
			MoveTextBox(hWnd);
			// MoveConsoleLineBox(hWnd);
			break;


		case WM_SIZE:

			// MoveWindow(hEdit, 0, 0, LOWORD(lp), HIWORD(lp), TRUE);
			break;

		case WM_NOTIFY:
			if (((LPNMHDR)lp)->code == EN_MSGFILTER) {
			}

			break;

		case WM_COMMAND:

			// 正規表現ボックスが書き換えられた直後
			if (lp && (HWND)lp==hRgxEdit && HIWORD(wp) == EN_UPDATE) {
				GetWindowText(hRgxEdit , hRgxEditContent , sizeof(hRgxEditContent));
				ReWriteAllRichEdit( hEdit );
			}
			switch (LOWORD(wp)) {
				case IDM_END:
					SendMessage(hWnd, WM_CLOSE, 0, 0);
					break;
				case IDM_FONT:
					SetForegroundFont(hEdit);
					break;
				case RECIEVEBUTTON_ID:
					SwitchRecieveStatus();
					break;
				case CLEARBUTTON_ID:
					ClearRichEdit(hEdit);
					break;
			}
			break;

		case WM_CLOSE:
			// スレッドのクローズ
			if (hTh != NULL) {
				CloseHandle(hTh);
			}
			DeleteTextBox();
			DeleteRegexBox();
			DeleteRecieveButton();
			DeleteClearButton();
			DeleteRegexBoxTextLabel();
			// DeleteConsoleLineBox();
			DestroyWindow(hWnd);

			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			return (DefWindowProc(hWnd, msg, wp, lp));
	}
	return 0L;
}
