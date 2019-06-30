#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <vector>
#include <map>
#include <process.h>
#include "autocompletewindow.h"
#include "outputdebugstream.h"
#include "hookwin32api.h"
#include "autocomletehelptips.h"

#include "OnigRegex.h"

using namespace std;


//-----------------------------------------------------------------------------------------------------------------------------------
// SendMessageA / SendMessageW
//-------------------------------------------------------------------------------------------------------------------------------------
extern "C" /*__declspec(dllexport)*/ PROC hook(char *szModuleName, char *szFuncName, void *szFuncAddress);


// �t�b�N����֐��̃v���g�^�C�v���`
using PFNSENDMESSAGEA = LRESULT(WINAPI *)(HWND, UINT, WPARAM, LPARAM);

PROC pfnPrevSendMessageA = NULL; // �P�O��SendMessage�֐��B

int Lock_SendMessageA = FALSE;

// ����SendMessageA�ƑS�������`
extern "C" __declspec(dllexport) LRESULT WINAPI Hook_SendMessageA(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!Lock_SendMessageA) {
		Lock_SendMessageA = TRUE;

		//-------------------------------------------- �������牺�̓I���W�i�����Ăяo������
		// �����R�[�h�̓���
		if (uMsg == 0x61E) {
			OutputDebugStream("������SendmessageA %x, %x, %x", hWnd, wParam, lParam);
			OnHidemaruApplicationKeyDown(hWnd, wParam, lParam);
			wLastKeyDown = wParam;
		}
		/*
		if (hWnd == hWndAutoCompleteListBox) {
		if (Msg == LB_GETTEXT) {
		}
		}
		else if (hWnd == hWndAutoCompleteListBox) {
		OutputDebugStream("�� %x, %x, %x\n", Msg, wParam, lParam);
		}
		*/

	}
	Lock_SendMessageA = FALSE;
	return ((PFNSENDMESSAGEA)pfnPrevSendMessageA)(hWnd, uMsg, wParam, lParam);
}

map<int, string> stringmap;

// �t�b�N����֐��̃v���g�^�C�v���`
using PFNSENDMESSAGEW = LRESULT(WINAPI *)(HWND, UINT, WPARAM, LPARAM);

PROC pfnPrevSendMessageW = NULL; // �P�O��SendMessage�֐��B
int Lock_SendMessageW = FALSE;

// ����SendMessageA�ƑS�������`
extern "C" __declspec(dllexport) LRESULT WINAPI Hook_SendMessageW(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!Lock_SendMessageW) {
		Lock_SendMessageW = TRUE;

		//-------------------------------------------- �������牺�̓I���W�i�����Ăяo������
		// �����R�[�h�̓���
		if (uMsg == 0x61E) {
			OutputDebugStream("������SendmessageW %x, %x, %x", hWnd, wParam, lParam);
			OnHidemaruApplicationKeyDown(hWnd, wParam, lParam);
			wLastKeyDown = wParam;
		}
		if (stringmap[0x0180] == "") {
			stringmap[0x0180] = "LB_ADDSTRING";
			stringmap[0x0181] = "LB_INSERTSTRING";
			stringmap[0x0182] = "LB_DELETESTRING";
			stringmap[0x0183] = "LB_SELITEMRANGEEX";
			stringmap[0x0184] = "LB_RESETCONTENT";
			stringmap[0x0185] = "LB_SETSEL";
			stringmap[0x0186] = "LB_SETCURSEL";
			stringmap[0x0187] = "LB_GETSEL";
			stringmap[0x0188] = "LB_GETCURSEL";
			stringmap[0x0189] = "LB_GETTEXT";
			stringmap[0x018A] = "LB_GETTEXTLEN";
			stringmap[0x018B] = "LB_GETCOUNT";
			stringmap[0x018C] = "LB_SELECTSTRING";
			stringmap[0x018D] = "LB_DIR";
			stringmap[0x018E] = "LB_GETTOPINDEX";
			stringmap[0x018F] = "LB_FINDSTRING";
			stringmap[0x0190] = "LB_GETSELCOUNT";
			stringmap[0x0191] = "LB_GETSELITEMS";
			stringmap[0x0192] = "LB_SETTABSTOPS";
			stringmap[0x0193] = "LB_GETHORIZONTALEXTENT";
			stringmap[0x0194] = "LB_SETHORIZONTALEXTENT";
			stringmap[0x0195] = "LB_SETCOLUMNWIDTH";
			stringmap[0x0196] = "LB_ADDFILE";
			stringmap[0x0197] = "LB_SETTOPINDEX";
			stringmap[0x0198] = "LB_GETITEMRECT";
			stringmap[0x0199] = "LB_GETITEMDATA";
			stringmap[0x019A] = "LB_SETITEMDATA";
			stringmap[0x019B] = "LB_SELITEMRANGE";
			stringmap[0x019C] = "LB_SETANCHORINDEX";
			stringmap[0x019D] = "LB_GETANCHORINDEX";
			stringmap[0x019E] = "LB_SETCARETINDEX";
			stringmap[0x019F] = "LB_GETCARETINDEX";
			stringmap[0x01A0] = "LB_SETITEMHEIGHT";
			stringmap[0x01A1] = "LB_GETITEMHEIGHT";
			stringmap[0x01A2] = "LB_FINDSTRINGEXACT";
			stringmap[0x01A5] = "LB_SETLOCALE";
			stringmap[0x01A6] = "LB_GETLOCALE";
			stringmap[0x01A7] = "LB_SETCOUNT";
			stringmap[0x01A8] = "LB_INITSTORAGE";
			stringmap[0x01A9] = "LB_ITEMFROMPOINT";
		}
		if (hWnd == hWndAutoCompleteListBox) {
			/*
			PFNPROC curproc = (PFNPROC)GetWindowLong(hWndAutoCompleteListBox, GWL_WNDPROC);
			if (curproc != WindowProcAutoCompleteListBox) {
			pOriginalWIndowProcAutoCompleteListBox = curproc;
			SetWindowLong(hWndAutoCompleteListBox, GWL_WNDPROC, (LONG)WindowProcAutoCompleteListBox);
			}
			*/
			/*
			0x0180 LB_ADDSTRING            0x0180
			0x0181 LB_INSERTSTRING         0x0181
			0x0182 LB_DELETESTRING         0x0182
			0x0183 LB_SELITEMRANGEEX       0x0183
			0x0184 LB_RESETCONTENT         0x0184
			0x0185 LB_SETSEL               0x0185
			0x0186 LB_SETCURSEL            0x0186
			0x0187 LB_GETSEL               0x0187
			0x0188 LB_GETCURSEL            0x0188
			0x0189 LB_GETTEXT              0x0189
			0x018A LB_GETTEXTLEN           0x018A
			0x018B LB_GETCOUNT             0x018B
			0x018C LB_SELECTSTRING         0x018C
			0x018D LB_DIR                  0x018D
			0x018E LB_GETTOPINDEX          0x018E
			0x018F LB_FINDSTRING           0x018F
			0x0190 LB_GETSELCOUNT          0x0190
			0x0191 LB_GETSELITEMS          0x0191
			0x0192 LB_SETTABSTOPS          0x0192
			0x0193 LB_GETHORIZONTALEXTENT  0x0193
			0x0194 LB_SETHORIZONTALEXTENT  0x0194
			0x0195 LB_SETCOLUMNWIDTH       0x0195
			0x0196 LB_ADDFILE              0x0196
			0x0197 LB_SETTOPINDEX          0x0197
			0x0198 LB_GETITEMRECT          0x0198
			0x0199 LB_GETITEMDATA          0x0199
			0x019A LB_SETITEMDATA          0x019A
			0x019B LB_SELITEMRANGE         0x019B
			0x019C LB_SETANCHORINDEX       0x019C
			0x019D LB_GETANCHORINDEX       0x019D
			0x019E LB_SETCARETINDEX        0x019E
			0x019F LB_GETCARETINDEX        0x019F
			0x01A0 LB_SETITEMHEIGHT        0x01A0
			0x01A1 LB_GETITEMHEIGHT        0x01A1
			0x01A2 LB_FINDSTRINGEXACT      0x01A2
			0x01A5 LB_SETLOCALE            0x01A5
			0x01A6 LB_GETLOCALE            0x01A6
			0x01A7 LB_SETCOUNT             0x01A7
			0x01A8 LB_INITSTORAGE          0x01A8
			0x01A9 LB_ITEMFROMPOINT        0x01A9
			*/
			/*
			if (uMsg == LBN_KILLFOCUS) {
			OutputDebugStream("�������͕⊮����L�[�{�[�h�t�H�[�J�X��������");
			}
			*/
			// �ǉ����n�܂���
			if (uMsg == LB_INSERTSTRING || uMsg == LB_ADDSTRING) {
				bAutoCompleteListBoxAddMode = 1;
			}
			// �ǉ����̃t���O�������Ă���̂ɁA�ǉ����߂����Ă��Ȃ�
			else if (bAutoCompleteListBoxAddMode == 1) {
				OnHidemaruListBoxAddComplete(hWndAutoCompleteListBox, uMsg, wParam, lParam);
				bAutoCompleteListBoxAddMode = 2;
			}
			if (uMsg == LB_GETTEXT && bAutoCompleteListBoxAddMode > 1 || uMsg == LB_GETTEXTLEN && bAutoCompleteListBoxAddMode && wParam == 0) {
				int cursor_index = ((PFNSENDMESSAGEA)pfnPrevSendMessageA)(hWnd, LB_GETCURSEL, 0, 0);
				int carret_index = ((PFNSENDMESSAGEA)pfnPrevSendMessageA)(hWnd, LB_GETCARETINDEX, 0, 0);

				// �I�΂�Ă��Ȃ�
				if (cursor_index == -1) {
					/*
					// wParam��0���傫�Ȓl�Ȃ�A�I��łȂ��̂ł͂Ȃ��A�Ō�̍��ڂ�I�����Ă��邩������Ȃ�
					if (wParam > 0) {
					// ���𒲂ׂ�
					int allcount = ((PFNSENDMESSAGEA)pfnPrevSendMessageA)(hWnd, LB_GETCOUNT, 0, 0);
					// wParam�ƌ�-1�������Ȃ�A�Ō�̒l��I�����Ă���
					if (wParam == allcount - 1) {
					// �Ō�̒l���w�����Ă��邱�ƂƏC������B
					cursor_index = allcount - 1;
					}
					}
					*/
				}

				if (cursor_index == -1) {
					;
				}
				else {
					char szBuffer[4096];
					((PFNSENDMESSAGEA)pfnPrevSendMessageA)(hWnd, LB_GETTEXT, cursor_index, (LPARAM)szBuffer);
					OutputDebugStream("�w���v�`�b�v�̃^�C�~���O!!%x, %x, %x, �J�[�\��%d, �J���b�g%d\n", uMsg, wParam, lParam, cursor_index, carret_index);

					RECT rect;
					GetWindowRect(hWnd, &rect);  // �E�B���h�E�̍��W�l
					OutputDebugStream("�����������W%d,%d,%d,%d\n", rect.left, rect.top, rect.bottom, rect.right);

					int iItemHeight = ((PFNSENDMESSAGEA)pfnPrevSendMessageA)(hWnd, LB_GETITEMHEIGHT, 0, 0);

					if (cursor_index == 0) {
						// ���s+�X�y�[�X�݂����Ȃ̂�����
						Matches m;
						if (OnigMatch(szBuffer, "^(.+)   \\(.+?=.+?\\)$", &m)) {
							if (m[1].size() > 0) {
								strcpy(szBuffer, m[1].data()); // m[1]�͕���������Ȃ̂ŁA��΃o�b�t�@�ɗ]�T������B
							}
						}
					}

					AutoCompleteHelpTip::OnListBoxSelectedIndexChanged(hWnd, cursor_index, szBuffer, rect, iItemHeight);
				}
			}

			else if (stringmap[uMsg].size() > 1) {
				OutputDebugStream("�R�}���h ��%s\n", stringmap[uMsg].data());
				OutputDebugStream("�R�}���h %x, %x, %x\n", uMsg, wParam, lParam);
			}
			else {
				OutputDebugStream("�� ��%x\n", uMsg);
				OutputDebugStream("�� %x, %x, %x\n", uMsg, wParam, lParam);
			}
		}
		else if (hWnd == hWndAutoCompleteListBox) {
			OutputDebugStream("�� %x, %x, %x\n", uMsg, wParam, lParam);
		}
	}

	Lock_SendMessageW = FALSE;
	return ((PFNSENDMESSAGEW)pfnPrevSendMessageW)(hWnd, uMsg, wParam, lParam);
}
//-----------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------


// �t�b�N����֐��̃v���g�^�C�v���`
using PFNCREATEWINDOWEXA = HWND (WINAPI *)(DWORD dwExStyle,      // �g���E�B���h�E�X�^�C��
	LPCTSTR lpClassName,  // �o�^����Ă���N���X��
	LPCTSTR lpWindowName, // �E�B���h�E��
	DWORD dwStyle,        // �E�B���h�E�X�^�C��
	int x,                // �E�B���h�E�̉������̈ʒu
	int y,                // �E�B���h�E�̏c�����̈ʒu
	int nWidth,           // �E�B���h�E�̕�
	int nHeight,          // �E�B���h�E�̍���
	HWND hWndParent,      // �e�E�B���h�E�܂��̓I�[�i�[�E�B���h�E�̃n���h��
	HMENU hMenu,          // ���j���[�n���h���܂��͎q���ʎq
	HINSTANCE hInstance,  // �A�v���P�[�V�����C���X�^���X�̃n���h��
	LPVOID lpParam        // �E�B���h�E�쐬�f�[�^
);

PROC pfnPrevCreateWindowExA = NULL; // �P�O��SendMessage�֐��B

int Lock_CreateWindowExA = FALSE;

// ����SendMessageA�ƑS�������`
extern "C" __declspec(dllexport) HWND WINAPI Hook_CreateWindowExA(DWORD dwExStyle,      // �g���E�B���h�E�X�^�C��
	LPCTSTR lpClassName,  // �o�^����Ă���N���X��
	LPCTSTR lpWindowName, // �E�B���h�E��
	DWORD dwStyle,        // �E�B���h�E�X�^�C��
	int x,                // �E�B���h�E�̉������̈ʒu
	int y,                // �E�B���h�E�̏c�����̈ʒu
	int nWidth,           // �E�B���h�E�̕�
	int nHeight,          // �E�B���h�E�̍���
	HWND hWndParent,      // �e�E�B���h�E�܂��̓I�[�i�[�E�B���h�E�̃n���h��
	HMENU hMenu,          // ���j���[�n���h���܂��͎q���ʎq
	HINSTANCE hInstance,  // �A�v���P�[�V�����C���X�^���X�̃n���h��
	LPVOID lpParam        // �E�B���h�E�쐬�f�[�^
	)
{
	//-------------------------------------------- �������牺�̓I���W�i�����Ăяo������
	HWND ret = ((PFNCREATEWINDOWEXA)pfnPrevCreateWindowExA)(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

	if (!Lock_CreateWindowExA) {
		Lock_CreateWindowExA = TRUE;

		HWND parent = GetParent(ret);
		if (parent) {
			char sz[100];
			GetClassName(parent, sz, sizeof(sz));
			if (strcmp(sz, "HidemaruAutocomp") == 0) {
				hWndAutoCompleteListBox = ret;
				OutputDebugStream("��A�̕�\n");
				GetClassName(ret, sz, sizeof(sz));

				OutputDebugStream(sz);
			}
		}
	}
	Lock_CreateWindowExA = FALSE;
	return ret;
}
//-----------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------


// �t�b�N����֐��̃v���g�^�C�v���`
using PFNCREATEWINDOWEXW = HWND(WINAPI *)(
	DWORD dwExStyle,      // �g���E�B���h�E�X�^�C��
	LPCTSTR lpClassName,  // �o�^����Ă���N���X��
	LPCTSTR lpWindowName, // �E�B���h�E��
	DWORD dwStyle,        // �E�B���h�E�X�^�C��
	int x,                // �E�B���h�E�̉������̈ʒu
	int y,                // �E�B���h�E�̏c�����̈ʒu
	int nWidth,           // �E�B���h�E�̕�
	int nHeight,          // �E�B���h�E�̍���
	HWND hWndParent,      // �e�E�B���h�E�܂��̓I�[�i�[�E�B���h�E�̃n���h��
	HMENU hMenu,          // ���j���[�n���h���܂��͎q���ʎq
	HINSTANCE hInstance,  // �A�v���P�[�V�����C���X�^���X�̃n���h��
	LPVOID lpParam        // �E�B���h�E�쐬�f�[�^
	);

PROC pfnPrevCreateWindowExW = NULL; // �P�O��SendMessage�֐��B

int Lock_CreateWindowExW = FALSE;

// ����SendMessageA�ƑS�������`
extern "C" __declspec(dllexport) HWND WINAPI Hook_CreateWindowExW(DWORD dwExStyle,      // �g���E�B���h�E�X�^�C��
	LPCTSTR lpClassName,  // �o�^����Ă���N���X��
	LPCTSTR lpWindowName, // �E�B���h�E��
	DWORD dwStyle,        // �E�B���h�E�X�^�C��
	int x,                // �E�B���h�E�̉������̈ʒu
	int y,                // �E�B���h�E�̏c�����̈ʒu
	int nWidth,           // �E�B���h�E�̕�
	int nHeight,          // �E�B���h�E�̍���
	HWND hWndParent,      // �e�E�B���h�E�܂��̓I�[�i�[�E�B���h�E�̃n���h��
	HMENU hMenu,          // ���j���[�n���h���܂��͎q���ʎq
	HINSTANCE hInstance,  // �A�v���P�[�V�����C���X�^���X�̃n���h��
	LPVOID lpParam        // �E�B���h�E�쐬�f�[�^
	)
{
	//-------------------------------------------- �������牺�̓I���W�i�����Ăяo������
	HWND ret = ((PFNCREATEWINDOWEXA)pfnPrevCreateWindowExW)(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
	if (!Lock_CreateWindowExW) {
		Lock_CreateWindowExW = TRUE;

		HWND parent = GetParent(ret);
		if (parent) {
			char sz[100];
			GetClassName(parent, sz, sizeof(sz));
			if (strcmp(sz, "HidemaruAutocomp") == 0) {
				hWndAutoCompleteListBox = ret;
				OutputDebugStream("��W�̕�\n");
				GetClassName(ret, sz, sizeof(sz));
				OutputDebugStream(sz);
				OnHidemaruListBoxAppear(hWndAutoCompleteListBox, wLastKeyDown);
			}
		}
	}

	Lock_CreateWindowExW = FALSE;

	return ret;
}
//-----------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------



// �t�b�N����֐��̃v���g�^�C�v���`
using PFNDESTROYWINDOW = BOOL(WINAPI *)(
	HWND hWnd  // �j������E�B���h�E�̃n���h��
);

PROC pfnPrevDestroyWindow = NULL; // �P�O��DestroyWindow�֐��B

int Lock_DestroyWindow = FALSE;

// ����SendMessageA�ƑS�������`
extern "C" __declspec(dllexport) BOOL WINAPI Hook_DestroyWindow(HWND hWnd)  // �j������E�B���h�E�̃n���h��
	
{
	if (!Lock_DestroyWindow) {
		Lock_DestroyWindow = TRUE;

		char cname[100];
		GetClassName(hWnd, cname, sizeof(cname));
		if (hWnd == hWndAutoCompleteListBox || strcmp(cname, "HidemaruAutocomp") == 0) {

			// �����⊮���̃w���v�`�b�v�̏���
			AutoCompleteHelpTip::OnDestroy(hWndAutoCompleteListBox);

			hWndAutoCompleteListBox = NULL;
			ResetAutoCompleteListBoxIndex();
			OutputDebugStream("����DestroyAutoCompleteWindow\n");
		}
	}

	Lock_DestroyWindow = FALSE;
	//-------------------------------------------- �������牺�̓I���W�i�����Ăяo������
	return ((PFNDESTROYWINDOW)pfnPrevDestroyWindow)(hWnd);
}
//-----------------------------------------------------------------------------------------------------------------------------------
/*
// �t�b�N����֐��̃v���g�^�C�v���`
using PFNCREATEFILEA = HANDLE (WINAPI *)(
	LPCTSTR lpFileName,                         // �t�@�C����
	DWORD dwDesiredAccess,                      // �A�N�Z�X���[�h
	DWORD dwShareMode,                          // ���L���[�h
	LPSECURITY_ATTRIBUTES lpSecurityAttributes, // �Z�L�����e�B�L�q�q
	DWORD dwCreationDisposition,                // �쐬���@
	DWORD dwFlagsAndAttributes,                 // �t�@�C������
	HANDLE hTemplateFile                        // �e���v���[�g�t�@�C���̃n���h��
	);

PROC pfnPrevCreateFileA = NULL; // �P�O��DestroyWindow�֐��B
// ����SendMessageA�ƑS�������`
extern "C" __declspec(dllexport) HANDLE WINAPI Hook_CreateFileA(
	LPCTSTR lpFileName,                         // �t�@�C����
	DWORD dwDesiredAccess,                      // �A�N�Z�X���[�h
	DWORD dwShareMode,                          // ���L���[�h
	LPSECURITY_ATTRIBUTES lpSecurityAttributes, // �Z�L�����e�B�L�q�q
	DWORD dwCreationDisposition,                // �쐬���@
	DWORD dwFlagsAndAttributes,                 // �t�@�C������
	HANDLE hTemplateFile                        // �e���v���[�g�t�@�C���̃n���h��
	)
{
	OutputDebugStream("��CreateFileA");
	OutputDebugStream("%s", lpFileName);
	OutputDebugStream("\n");
	//-------------------------------------------- �������牺�̓I���W�i�����Ăяo������
	HANDLE ret = ((PFNCREATEFILEA)pfnPrevCreateFileA)(
			lpFileName,
			dwDesiredAccess,
			dwShareMode,
			lpSecurityAttributes,
			dwCreationDisposition,
			dwFlagsAndAttributes,
			hTemplateFile);
	return ret;
}



// �t�b�N����֐��̃v���g�^�C�v���`
using PFNCREATEFILEW = HANDLE(WINAPI *)(
	LPCTSTR lpFileName,                         // �t�@�C����
	DWORD dwDesiredAccess,                      // �A�N�Z�X���[�h
	DWORD dwShareMode,                          // ���L���[�h
	LPSECURITY_ATTRIBUTES lpSecurityAttributes, // �Z�L�����e�B�L�q�q
	DWORD dwCreationDisposition,                // �쐬���@
	DWORD dwFlagsAndAttributes,                 // �t�@�C������
	HANDLE hTemplateFile                        // �e���v���[�g�t�@�C���̃n���h��
	);

PROC pfnPrevCreateFileW = NULL; // �P�O��DestroyWindow�֐��B
// ����SendMessageA�ƑS�������`
extern "C" __declspec(dllexport) HANDLE WINAPI Hook_CreateFileW(
	LPCTSTR lpFileName,                         // �t�@�C����
	DWORD dwDesiredAccess,                      // �A�N�Z�X���[�h
	DWORD dwShareMode,                          // ���L���[�h
	LPSECURITY_ATTRIBUTES lpSecurityAttributes, // �Z�L�����e�B�L�q�q
	DWORD dwCreationDisposition,                // �쐬���@
	DWORD dwFlagsAndAttributes,                 // �t�@�C������
	HANDLE hTemplateFile                        // �e���v���[�g�t�@�C���̃n���h��
	)
{
	OutputDebugStream("��CreateFileW");

	//�ϊ�������i�[�o�b�t�@
	char	wStrC[512];

	size_t wLen = 0;
	errno_t err = 0;

	//�ϊ�
	err = wcstombs_s(&wLen, wStrC, 512, (const wchar_t *)lpFileName, _TRUNCATE);

	OutputDebugStream("%s", wStrC);
	OutputDebugStream("\n");
	//-------------------------------------------- �������牺�̓I���W�i�����Ăяo������
	HANDLE ret = ((PFNCREATEFILEA)pfnPrevCreateFileW)(
		lpFileName,
		dwDesiredAccess,
		dwShareMode,
		lpSecurityAttributes,
		dwCreationDisposition,
		dwFlagsAndAttributes,
		hTemplateFile);
	return ret;
}
*/

BOOL HookWin32apiFuncs_Done = FALSE;
void HookWin32apiFuncs() {

	if (!HookWin32apiFuncs_Done) {
		HookWin32apiFuncs_Done = TRUE;

		pfnPrevSendMessageA    = hook_win32api("user32.dll", "SendMessageA", Hook_SendMessageA);
		pfnPrevSendMessageW    = hook_win32api("user32.dll", "SendMessageW", Hook_SendMessageW);
		pfnPrevCreateWindowExA = hook_win32api("user32.dll", "CreateWindowExA", Hook_CreateWindowExA);
		pfnPrevCreateWindowExW = hook_win32api("user32.dll", "CreateWindowExW", Hook_CreateWindowExW);
		pfnPrevDestroyWindow   = hook_win32api("user32.dll", "DestroyWindow", Hook_DestroyWindow);
		// pfnPrevCreateFileA     = hook_win32api("kernel32.dll", "CreateFileA", Hook_CreateFileA);
		// pfnPrevCreateFileW     = hook_win32api("kernel32.dll", "CreateFileW", Hook_CreateFileW);
	}
}