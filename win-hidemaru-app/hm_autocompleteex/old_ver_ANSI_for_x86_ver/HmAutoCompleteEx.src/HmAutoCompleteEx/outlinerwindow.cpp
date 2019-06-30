#include <windows.h>

#include "outputdebugstream.h"

// ★★★
// リストボックスは、アウトライナーに使われる。
void AnalizeOutlinerListBox(HWND wnd) {

	OutputDebugStream("アウトライナ %x", wnd);
	char szTitle[1024];
	GetWindowText(wnd, szTitle, sizeof(szTitle));
	OutputDebugStream("アウトライナ タイトル%s", szTitle);

	OutputDebugStream("アウトライナ %x", wnd);
	int lb_getcursel = SendMessageA(wnd, LB_GETCURSEL, 0, 0);
	OutputDebugStream("現在のアウトライナ内のパス上のインデックス%d", lb_getcursel);
	int lb_getcount = SendMessageA(wnd, LB_GETCOUNT, 0, 0);
	OutputDebugStream("現在のアウトライナ内のインデックス総数%d", lb_getcount);
	int lb_gettextlen_of_cursel = SendMessageA(wnd, LB_GETTEXTLEN, lb_getcursel, 0);
	OutputDebugStream("現在のアウトライナ内の現在のインデックス内の文字数%d", lb_gettextlen_of_cursel);
}
