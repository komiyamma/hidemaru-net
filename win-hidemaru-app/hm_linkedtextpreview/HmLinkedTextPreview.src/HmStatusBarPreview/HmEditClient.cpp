/*
 * Copyright(C) 2017 Akitsugu Komiyama
 * under the MIT License
 */ 

#include <windows.h>
#include "OutputDebugStream.h"
#include "HmStatusBarPreviewFormController.h"
#include "HmStatusBarText.h"


HWND hHm32Client = NULL; // �G�f�B�g

namespace HidemaruEditClient {

	bool IsInMouse(HWND hWnd) {
		// �G�f�B�����̈�̋�`
		RECT rect;
		GetWindowRect(hWnd, &rect);

		// ���݂̃}�E�X�̈ʒu
		POINT po;
		GetCursorPos(&po);

		HWND hWndUnderMouse = WindowFromPoint(po);
		char name[256] = "";
		GetClassNameA(hWndUnderMouse, name, _countof(name));
		
		// �G�f�B�^�̈��ɖ����Ȃ瑦���Ƀ_��
		if (strcmp(name, "HM32CLIENT") != 0) {
			return false;
		}

		// �G�f�B�����̈����ɏo���B���ɏo���B
		if (po.y < rect.top + 5 || po.x < rect.left + 5) {
			return false;
		}
		if (po.y > rect.bottom-16 || po.x > rect.right-16) { // 16�Ă͔̂�r�I�͂ݏo���Ԃłŏ�����悤��
			return false;
		}

		return true;
	}

};