/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#include <windows.h>

using namespace std;


#pragma unmanaged
BOOL WINAPI DllMain(HINSTANCE hinstDll, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason) {

	case DLL_PROCESS_ATTACH: // DLL���v���Z�X�̃A�h���X��ԂɃ}�b�s���O���ꂽ�B
		break;
	case DLL_THREAD_ATTACH: // �X���b�h���쐬����悤�Ƃ��Ă���B
		break;
	case DLL_THREAD_DETACH: // �X���b�h���j������悤�Ƃ��Ă���B
		break;
	case DLL_PROCESS_DETACH: // DLL�̃}�b�s���O����������悤�Ƃ��Ă���B
		break;

	}

	return TRUE;
}

