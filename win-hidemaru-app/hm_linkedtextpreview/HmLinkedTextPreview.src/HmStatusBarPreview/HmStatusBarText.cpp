/*
 * Copyright(C) 2017 Akitsugu Komiyama
 * under the MIT License
 */ 

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

	// �X�e�[�^�X�̕�����𓾂�
	wstring GetStatusBarText(HWND hWnd) {

		// �����̃`�F�b�N�B����������ΏۊO�B1024�����܂�
		LRESULT lLen = SendMessage(hWnd, SB_GETTEXTLENGTH, 0, 0);
		if (!lLen) {
			// OutputDebugStream(L"����");
			return L"";
		}
		if (lLen >= 1024) {
			// OutputDebugStream(L"������");
			return L"";
		}

		// �ꉞ�o�b�t�@�ɂ͗]�T�������Ă����B
		wchar_t szBuffer[4096] = L"";
		LRESULT lPart = SendMessage(hWnd, SB_GETTEXT, 0, (LPARAM)szBuffer);
		szBuffer[4095] = NULL;

		return wstring(szBuffer);
	}

	void TextChanged(wstring text, HWND hWnd) {

		// OutputDebugStream(text.c_str());

		// ���łɐH������Ă�����A��U����
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

		// �O��̃o�b�t�@�[�̓��e�ƍ��A�X�e�[�^�X�o�[�ɂ�����e���قȂ�ꍇ
		if (previousStatusBarText != curStatsuBarText) {
			// �e�L�X�g�̕ω��C�x���g�n���h���ɗ���
			TextChanged(curStatsuBarText, hWnd);
			previousStatusBarText = curStatsuBarText;
		}

		// �O��Ɠ�����������A�\�����p���B�`��(�ʒu)�����X�V
		else {
			HmStatsusBarPreviewFormController::Update();
		}
	}

}