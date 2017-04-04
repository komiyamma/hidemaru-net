#include <windows.h>

#pragma unmanaged

#pragma data_seg("HM_SHARED_STATUSBAR_PREVIEW_SEG")
HWND hCurrentForeHidemaruHandle = NULL;
#pragma data_seg()

#pragma managed