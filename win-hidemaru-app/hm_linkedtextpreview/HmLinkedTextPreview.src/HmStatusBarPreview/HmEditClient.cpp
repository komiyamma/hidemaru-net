/*
 * Copyright(C) 2017 Akitsugu Komiyama
 * under the MIT License
 */ 

#include <windows.h>
#include "OutputDebugStream.h"
#include "HmStatusBarPreviewFormController.h"
#include "HmStatusBarText.h"


HWND hHm32Client = NULL; // エディト

namespace HidemaruEditClient {

	bool IsInMouse(HWND hWnd) {
		// エディった領域の矩形
		RECT rect;
		GetWindowRect(hWnd, &rect);

		// 現在のマウスの位置
		POINT po;
		GetCursorPos(&po);

		HWND hWndUnderMouse = WindowFromPoint(po);
		char name[256] = "";
		GetClassNameA(hWndUnderMouse, name, _countof(name));
		
		// エディタ領域上に無いなら即座にダメ
		if (strcmp(name, "HM32CLIENT") != 0) {
			return false;
		}

		// エディった領域より上に出た。左に出た。
		if (po.y < rect.top + 5 || po.x < rect.left + 5) {
			return false;
		}
		if (po.y > rect.bottom-16 || po.x > rect.right-16) { // 16てのは比較的はみ出る手間でで消えるように
			return false;
		}

		return true;
	}

};