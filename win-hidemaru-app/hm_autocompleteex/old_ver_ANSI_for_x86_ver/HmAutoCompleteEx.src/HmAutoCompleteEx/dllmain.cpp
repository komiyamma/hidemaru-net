#include <windows.h>
#include <stdio.h>

#include "outputdebugstream.h"
#include "autocompletewindow.h"
#include "hookwin32api.h"
#include "outlinerwindow.h"
#include "programmemory.h"
#include "OnigRegex.h"

using namespace std;


HANDLE curProcess = NULL;
char szHidemaru[MAX_PATH*2];
HINSTANCE hInst = NULL;


#pragma unmanaged
BOOL WINAPI DllMain(HINSTANCE hinstDll, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason) {

	case DLL_PROCESS_ATTACH: // DLL���v���Z�X�̃A�h���X��ԂɃ}�b�s���O���ꂽ�B
		{
			// �S�Ԃ̏�����
			onig_init();
			hInst = hinstDll;
			curProcess = ::GetCurrentProcess();
			GetModuleFileName(NULL, szHidemaru, sizeof(szHidemaru));
		}
		break;

	case DLL_THREAD_ATTACH: // �X���b�h���쐬����悤�Ƃ��Ă���B
		break;

	case DLL_THREAD_DETACH: // �X���b�h���j������悤�Ƃ��Ă���B
		break;

	case DLL_PROCESS_DETACH: // DLL�̃}�b�s���O����������悤�Ƃ��Ă���B

		// �S�Ԃ̏I��
		onig_end();

		break;

	}

	return TRUE;
}

