/*
 * Copyright(C) 2017 Akitsugu Komiyama
 * under the MIT License
 */ 


#include <windows.h>
#include <process.h>
#include <tchar.h>
#include <string>

#include "HmStatusBarPreviewFormController.h"
#include "HmStatusBarText.h"
#include "HmEditClient.h"
#include "HmCursor.h"
#include "OutputDebugStream.h"
#include "hidemaruexe_export.h"


using namespace std;

CHidemaruExeExport HMEXE;


HWND hmWndHandle = NULL;

BOOL CALLBACK EnumChildProc(HWND hWnd, LPARAM lParam)
{
	wchar_t szTitle[1024];
	char szClass[1024];

	GetWindowText(hWnd, szTitle, _countof(szTitle));    // キャプションの取得
	GetClassNameA(hWnd, szClass, _countof(szClass));    // クラス文字列の取得 Wは無いので注意

	// 秀丸エディタ下のステータスバーウィンドウに対して…
	if (strcmp(szClass, "msctls_statusbar32") == 0) {

		HmStatusBarText::OnEnumChildProc(hWnd);
	}

	// 秀丸エディタ下の編集領域に対して…
	else if (strcmp(szClass, "HM32CLIENT") == 0) {

		// マウスがその中になければ…
		if (!HidemaruEditClient::IsInMouse(hWnd)) {
			HmStatsusBarPreviewFormController::Destroy();
		}

	}

	return TRUE;
}


// いずれかの秀丸がアクティブ
bool IsHidemaruActive() {
	HWND hWnd = GetForegroundWindow();
	char name[256] = "";
	GetClassNameA(hWnd, name, _countof(name));

	// デスクトップ版かもしくは、ストアアプリ版
	return strcmp(name, "Hidemaru32Class") == 0 || strcmp(name, "Hidemaru32Class_Appx") == 0;
}


const int iThreadInterval = 200;
extern HWND hCurrentForeHidemaruHandle;

// Ticスレッド関数
bool beExitThreadCheckStatusBar = false; // スレッドから抜けるべし
unsigned __stdcall ThreadCheckStatusBar(void *p)
{
	while(TRUE) {
		if (beExitThreadCheckStatusBar) {
			// OutputDebugStream( L"BREAK\n" );
			break;
		}

		// マウスの形がハンドかどうか
		bool isGlobalCursurIsHand = IsGlobalCursorIsHand();
		// ハンドじゃなかったら閉じる
		if (!isGlobalCursurIsHand) {

			// OutputDebugStream(L"違うから閉じる");
			HmStatsusBarPreviewFormController::Destroy();

			Sleep(iThreadInterval);
			continue;
		}

		// アクティブウィンドウは秀丸かどうか
		bool isHidemaruActive = IsHidemaruActive();
		if (!isHidemaruActive) {
			// OutputDebugStream(L"違うから閉じる");
			HmStatsusBarPreviewFormController::Destroy();

			Sleep(iThreadInterval);
			continue;

		}

		/*
		// なぜかウィンドウそのものではなくなっている
		if (!IsWindow(hCurrentForeHidemaruHandle)) {
			hCurrentForeHidemaruHandle = CHidemaruExeExport::GetCurWndHidemaru();
		}
		*/

		// 秀丸のハンドルはフォアグラウンドではない。
		if (hmWndHandle != hCurrentForeHidemaruHandle ) {
			// OutputDebugStream(L"違うから閉じる");
			HmStatsusBarPreviewFormController::Destroy();

			Sleep(iThreadInterval);
			continue;
		}

		// 秀丸の子ウィンドウを毎回探す。
		EnumChildWindows(hmWndHandle, EnumChildProc, (LPARAM)NULL);

		Sleep(iThreadInterval / 20); // ここまで来たら次回もスムーズ
		continue;


	}
	return 0;
}

HANDLE hThread = NULL;


extern "C" __declspec(dllexport) intptr_t SetHidemaruHandle(HWND pHmWndHandle) {
	// 秀丸のハンドルを代入。そのまま共用メモリにも格納しておく。intptr_t値１つなので、多分Mutexしなくて大丈夫だろう。
	hCurrentForeHidemaruHandle = hmWndHandle = pHmWndHandle;

	if (hThread == NULL) {
		// スレッドの作成
		hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadCheckStatusBar, NULL, 0, NULL);
	}
	return TRUE;
}

int HmBColor = 0xF5F5F5; // デフォルトはホワイトスモーク
extern "C" __declspec(dllexport) intptr_t SetHidemaruStrParam(wchar_t* param_name, wchar_t* param_value) {
	return TRUE;
}

extern "C" __declspec(dllexport) intptr_t SetHidemaruNumParam(wchar_t* param_name, intptr_t param_value) {
	if (wstring(param_name) == L"bcolor") {
		try {
			HmStatsusBarPreviewFormController::SetBColor((int)param_value);
		}
		catch (exception e) {
			OutputDebugStringA(e.what());
			return FALSE;
		}
	}
	return TRUE;
}


extern "C" __declspec(dllexport) intptr_t DllDetachFunc_After_Hm866() {
	// 終了するべきフラグを立てる
	beExitThreadCheckStatusBar = true;
	WaitForSingleObject(hThread, iThreadInterval * 2); // 最悪でもTicの２倍まで
	// スレッドの終了
	CloseHandle(hThread);

	hThread = NULL;

	// フォームも閉じる
	HmStatsusBarPreviewFormController::Destroy();

	// スレッドのストップ
	return TRUE;
}

