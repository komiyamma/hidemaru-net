#pragma once

#include <windows.h>


namespace Mutex {

	// Mutexオブジェクト用のハンドル
	extern HANDLE hMutex;

	// Mutex用の一意のラベルを得る(実態はこのdll名)。
	TCHAR* GetMutexLabel();

	// Mutexロック開始。誰かが取っていたら無限に待つ。
	void BgnMutexLock();

	// Mutexロック解放
	void EndMutexLock();
}