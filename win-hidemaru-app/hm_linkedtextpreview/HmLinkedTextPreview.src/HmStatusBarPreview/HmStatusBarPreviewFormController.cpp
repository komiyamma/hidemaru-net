/*
 * Copyright(C) 2017 Akitsugu Komiyama
 * under the MIT License
 */ 


#include "HmStatusBarPreviewForm.h"
#include "HmStatusBarText.h"

namespace HmStatsusBarPreviewFormController {

	void Create(HWND hWnd, String^ target) {
		// ウィンドウの作成
		if (HmStatsusBarPreviewForm::f == nullptr || HmStatsusBarPreviewForm::f->IsDisposed) {
			HmStatsusBarPreviewForm::f = gcnew HmStatsusBarPreviewForm(hWnd, target);
			if (HmStatsusBarPreviewForm::f->IsSouldBeShow()) {
				HmStatsusBarPreviewForm::f->Show();
			}
		}
	}

	void Update() {
		// ウィンドウの移動
		if (HmStatsusBarPreviewForm::f) {
			HmStatsusBarPreviewForm::f->MovePosition();
		}
	}

	void Destroy() {

		// 何時前のバーの内容のキャッシュはクリアしておく。
		HmStatusBarText::ClearCache();

		// ウィンドウの破棄
		if (HmStatsusBarPreviewForm::f) {
			HmStatsusBarPreviewForm::f->Close();
		}
		// このOnDestroyの後に、インスタンスが残っているのに任せるのは不安である。明示的に解放
		if (HmStatsusBarPreviewForm::f) {
			delete HmStatsusBarPreviewForm::f;
		}
	}

	void SetBColor(int rgb) {
		auto color = Color::FromArgb(rgb);
		HmStatsusBarPreviewForm::SetBColor(color);
	}
}

