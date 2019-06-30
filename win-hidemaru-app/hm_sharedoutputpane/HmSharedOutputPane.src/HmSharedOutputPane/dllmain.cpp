/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#include <windows.h>

#include "SharedExport.h"
#include <process.h>    /* _beginthread, _endthread */

HANDLE thHandle = NULL;
extern bool isMustBreakLoop;

// �G�ۂ�������Dll�ւ�FreeLibrary�̒��O�ɌĂяo��
extern "C" __declspec(dllexport) intptr_t DllDetachFunc_After_Hm866(){
	if (thHandle) {
		isMustBreakLoop = true;  // �I��邪�ǂ�
		WaitForSingleObject(thHandle, 70); // �x���Ƃ�40�~��������ΏI���\��
		isMustBreakLoop = false; // �I������̂Ńt���O�������ւƖ߂�
		CloseHandle(thHandle);
		thHandle = NULL;
	}

	return TRUE;
}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:

		// �A�E�g�v�b�g�p�l����DLL�̃|�C���^���m��
		hHmOutputPaneDLL = LoadLibrary(_T("HmOutputPane.dll"));
		if (hHmOutputPaneDLL) {
			// �A�E�g�v�b�g�y�C����Output�֐��̎擾
			// �A�E�g�v�b�g�y�C�����̂̃E�B���h�E�n���h���̎擾�B
			pOutputFunc = (PFNOUTPUT)GetProcAddress(hHmOutputPaneDLL, "Output");
			pOutputGetWindowFunc = (PFNGETWINDOWHANDLE)GetProcAddress(hHmOutputPaneDLL, "GetWindowHandle");

			// �X���b�h�̃X�^�[�g
			thHandle = (HANDLE)_beginthreadex(NULL, 0, OutputSharedMessage, 0, 0, NULL);
		}

		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	default:
		DllDetachFunc_After_Hm866();
		break;
	}
	return TRUE;
}

