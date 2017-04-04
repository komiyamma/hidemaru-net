#include <windows.h>
#include <string>
#include <commctrl.h>

#include "HmStatusBarPreviewFormController.h"
#include "HmEditClient.h"
#include "HmCursor.h"
#include "OutputDebugStream.h"
#include "HmStatusBarPreviewForm.h"
using namespace std;



namespace HmStatusBarText {

	wstring previousStatusBarText = L"";
	void ClearCache() {
		previousStatusBarText = L"";
	}

	// ステータスの文字列を得る
	wstring GetStatusBarText(HWND hWnd) {

		// 長さのチェック。長すぎたら対象外。1024文字まで
		LRESULT lLen = SendMessage(hWnd, SB_GETTEXTLENGTH, 0, 0);
		if (!lLen) {
			// OutputDebugStream(L"無い");
			return L"";
		}
		if (lLen >= 1024) {
			// OutputDebugStream(L"長すぎ");
			return L"";
		}

		// 一応バッファには余裕をもっておく。
		wchar_t szBuffer[4096] = L"";
		LRESULT lPart = SendMessage(hWnd, SB_GETTEXT, 0, (LPARAM)szBuffer);
		szBuffer[4095] = NULL;

		return wstring(szBuffer);
	}

	void TextChanged(wstring text, HWND hWnd) {

		// OutputDebugStream(text.c_str());

		// すでに食い違っていたら、一旦閉じる
		if (HmStatsusBarPreviewForm::f && HmStatsusBarPreviewForm::f->GetFileName() != gcnew String(text.c_str())) {
			HmStatsusBarPreviewFormController::Destroy();
		}

		if (text.length() > 0) {
			HmStatsusBarPreviewFormController::Create(hWnd, gcnew System::String(text.c_str()));
		}
		else {
			HmStatsusBarPreviewFormController::Destroy();
		}
	}

	void OnEnumChildProc(HWND hWnd) {

		wstring curStatsuBarText = GetStatusBarText(hWnd);

		// 前回のバッファーの内容と今、ステータスバーにある内容が異なる場合
		if (previousStatusBarText != curStatsuBarText) {
			// テキストの変化イベントハンドラに流す
			TextChanged(curStatsuBarText, hWnd);
			previousStatusBarText = curStatsuBarText;
		}

		// 前回と同じだったら、表示を継続。描画(位置)だけ更新
		else {
			HmStatsusBarPreviewFormController::Update();
		}
	}

}