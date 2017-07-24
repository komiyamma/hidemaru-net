#include "HmHtmlLive.h"
#include "HmHtmlFile.h"

//------------------------------------------------------------------------------------
#define MACRO_DLL extern "C" __declspec(dllexport)

// マクロから呼ばれる
MACRO_DLL intptr_t Show(HWND hWndHidemaru, int mode) {
	hmHtmlPreviewMode kMode = (hmHtmlPreviewMode)mode;
	// インスタンスをフォーム自身に保持させて…
	if (kMode == hmHtmlPreviewMode::Live) {
		HmHtmlBaseForm::form = gcnew HmHtmlLiveForm(hWndHidemaru);
	}
	else if (kMode == hmHtmlPreviewMode::File) {
		HmHtmlBaseForm::form = gcnew HmHtmlFileForm(hWndHidemaru);
	}

	// 想定外の数値
	else {
		HmHtmlBaseForm::form = gcnew HmHtmlLiveForm(hWndHidemaru);
	}

	// フォームを表示。
	HmHtmlBaseForm::form->Show();

	return TRUE;
}

// 秀丸の該当プロセスを閉じたとき
MACRO_DLL intptr_t Dispose() {

	try {
		// まだ残っていたら(フォームを手動で閉じていたら、残っていない)
		if (HmHtmlBaseForm::form != nullptr) {

			// 音を戻す
			CoInternetSetFeatureEnabled(FEATURE_DISABLE_NAVIGATION_SOUNDS, SET_FEATURE_ON_PROCESS, false);

			//監視を終了
			HmHtmlBaseForm::form->Stop();

			// 閉じてNull
			HmHtmlBaseForm::form->Close();
			HmHtmlBaseForm::form = nullptr;

			// 秀丸が閉じる前までに可能な限りマネージドのインスタンスを解放して不正エラーが出にくい状態へと持っていく。
			GC::Collect();
			GC::WaitForPendingFinalizers();
		}
	}
	catch (Exception^) {}

	return TRUE;
}

// 秀丸の該当プロセスを閉じたとき
MACRO_DLL intptr_t DllDetachFunc_After_Hm866() {
	return Dispose();
}