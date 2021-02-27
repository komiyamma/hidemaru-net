/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#include <windows.h>
#include <tchar.h>


extern HANDLE hDllModule;

namespace Mutex {

	// Mutex用のハンドル
	HANDLE hMutex = NULL;


	// dllが自分自身の名前を得る。Mutex用の識別名をこれにすることで、一意性を保つ。
	TCHAR* GetMutexLabel() {

		return _T("HmSharedOutputPane.dll");
	}

	static HANDLE hMutexLock = NULL;

	// Mutexロック開始。誰かが取っていたら無限に待つ。
	void BgnMutexLock() {
		// 排他開始
		hMutexLock = OpenMutex(MUTEX_ALL_ACCESS, FALSE, GetMutexLabel());

		if (hMutexLock) {
			// 他の所でロックされてたら待つ
			WaitForSingleObject(hMutexLock, INFINITE);
		}
	}

	// Mutexロック解放
	void EndMutexLock() {
		ReleaseMutex(hMutexLock);
	}

}


