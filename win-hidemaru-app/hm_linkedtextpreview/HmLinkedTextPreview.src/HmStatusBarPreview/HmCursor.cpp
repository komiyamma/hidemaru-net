#include <windows.h>
#include "OutputDebugStream.h"

bool IsGlobalCursorIsHand() {

	HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(NULL);

	CURSORINFO ci = { sizeof(CURSORINFO) };
	GetCursorInfo(&ci);

	// HAND
	if (ci.hCursor == (HCURSOR)65569 ) {
		return true;
	}
	else {
		return false;
	}

}
