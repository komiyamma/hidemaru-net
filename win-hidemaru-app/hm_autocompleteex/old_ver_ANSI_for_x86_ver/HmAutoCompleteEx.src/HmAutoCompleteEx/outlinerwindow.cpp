#include <windows.h>

#include "outputdebugstream.h"

// ������
// ���X�g�{�b�N�X�́A�A�E�g���C�i�[�Ɏg����B
void AnalizeOutlinerListBox(HWND wnd) {

	OutputDebugStream("�A�E�g���C�i %x", wnd);
	char szTitle[1024];
	GetWindowText(wnd, szTitle, sizeof(szTitle));
	OutputDebugStream("�A�E�g���C�i �^�C�g��%s", szTitle);

	OutputDebugStream("�A�E�g���C�i %x", wnd);
	int lb_getcursel = SendMessageA(wnd, LB_GETCURSEL, 0, 0);
	OutputDebugStream("���݂̃A�E�g���C�i���̃p�X��̃C���f�b�N�X%d", lb_getcursel);
	int lb_getcount = SendMessageA(wnd, LB_GETCOUNT, 0, 0);
	OutputDebugStream("���݂̃A�E�g���C�i���̃C���f�b�N�X����%d", lb_getcount);
	int lb_gettextlen_of_cursel = SendMessageA(wnd, LB_GETTEXTLEN, lb_getcursel, 0);
	OutputDebugStream("���݂̃A�E�g���C�i���̌��݂̃C���f�b�N�X���̕�����%d", lb_gettextlen_of_cursel);
}
