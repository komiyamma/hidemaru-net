#include <windows.h>
#include <tchar.h>
#include "outputdebugstream.h"

// ★★★
// リストボックスは、アウトライナーに使われる。
void AnalizeOutlinerListBox(HWND wnd) {

	OutputDebugStream(_T("アウトライナ %x"), wnd);
	TCHAR szTitle[1024];
	GetWindowText(wnd, szTitle, sizeof(szTitle));
	OutputDebugStream(_T("アウトライナ タイトル%s"), szTitle);

	OutputDebugStream(_T("アウトライナ %x"), wnd);
	LRESULT lb_getcursel = SendMessageA(wnd, LB_GETCURSEL, 0, 0);
	OutputDebugStream(_T("現在のアウトライナ内のパス上のインデックス%d"), lb_getcursel);
	LRESULT lb_getcount = SendMessageA(wnd, LB_GETCOUNT, 0, 0);
	OutputDebugStream(_T("現在のアウトライナ内のインデックス総数%d"), lb_getcount);
	LRESULT lb_gettextlen_of_cursel = SendMessageA(wnd, LB_GETTEXTLEN, lb_getcursel, 0);
	OutputDebugStream(_T("現在のアウトライナ内の現在のインデックス内の文字数%d"), lb_gettextlen_of_cursel);
}
