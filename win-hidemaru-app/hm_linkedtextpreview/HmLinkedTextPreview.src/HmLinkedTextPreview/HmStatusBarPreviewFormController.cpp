/*
 * Copyright(C) 2017 Akitsugu Komiyama
 * under the MIT License
 */ 


#include "HmStatusBarPreviewForm.h"
#include "HmStatusBarText.h"

namespace HmStatsusBarPreviewFormController {

	void Create(HWND hWnd, String^ target) {
		// �E�B���h�E�̍쐬
		if (HmStatsusBarPreviewForm::f == nullptr || HmStatsusBarPreviewForm::f->IsDisposed) {
			HmStatsusBarPreviewForm::f = gcnew HmStatsusBarPreviewForm(hWnd, target);
			if (HmStatsusBarPreviewForm::f->IsSouldBeShow()) {
				HmStatsusBarPreviewForm::f->Show();
			}
		}
	}

	void Update() {
		// �E�B���h�E�̈ړ�
		if (HmStatsusBarPreviewForm::f) {
			HmStatsusBarPreviewForm::f->MovePosition();
		}
	}

	void Destroy() {

		// �����O�̃o�[�̓��e�̃L���b�V���̓N���A���Ă����B
		HmStatusBarText::ClearCache();

		// �E�B���h�E�̔j��
		if (HmStatsusBarPreviewForm::f) {
			HmStatsusBarPreviewForm::f->Close();
		}
		// ����OnDestroy�̌�ɁA�C���X�^���X���c���Ă���̂ɔC����͕̂s���ł���B�����I�ɉ��
		if (HmStatsusBarPreviewForm::f) {
			delete HmStatsusBarPreviewForm::f;
		}
	}

	void SetBColor(int rgb) {
		auto color = Color::FromArgb(rgb);
		HmStatsusBarPreviewForm::SetBColor(color);
	}
}

