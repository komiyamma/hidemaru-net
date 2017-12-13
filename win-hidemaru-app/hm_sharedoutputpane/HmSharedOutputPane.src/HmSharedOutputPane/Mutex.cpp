/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#include <windows.h>
#include <tchar.h>


extern HANDLE hDllModule;

namespace Mutex {

	// Mutex�p�̃n���h��
	HANDLE hMutex = NULL;


	// dll���������g�̖��O�𓾂�BMutex�p�̎��ʖ�������ɂ��邱�ƂŁA��Ӑ���ۂB
	TCHAR* GetMutexLabel() {

		return _T("HmSharedOutputPane.dll");
	}

	static HANDLE hMutexLock = NULL;

	// Mutex���b�N�J�n�B�N��������Ă����疳���ɑ҂B
	void BgnMutexLock() {
		// �r���J�n
		hMutexLock = OpenMutex(MUTEX_ALL_ACCESS, FALSE, GetMutexLabel());

		if (hMutexLock) {
			// ���̏��Ń��b�N����Ă���҂�
			WaitForSingleObject(hMutexLock, INFINITE);
		}
	}

	// Mutex���b�N���
	void EndMutexLock() {
		ReleaseMutex(hMutexLock);
	}

}


