#pragma once

#include <windows.h>

namespace HmStatsusBarPreviewFormController {
	extern void Create(HWND hWnd, System::String^ target);
	extern void Update();
	extern void Destroy();

	extern void SetBColor(int rgb);
}
