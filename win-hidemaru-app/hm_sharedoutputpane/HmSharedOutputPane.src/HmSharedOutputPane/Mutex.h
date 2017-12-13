/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#pragma once

#include <windows.h>


namespace Mutex {

	// Mutex�I�u�W�F�N�g�p�̃n���h��
	extern HANDLE hMutex;

	// Mutex�p�̈�ӂ̃��x���𓾂�(���Ԃ͂���dll��)�B
	TCHAR* GetMutexLabel();

	// Mutex���b�N�J�n�B�N��������Ă����疳���ɑ҂B
	void BgnMutexLock();

	// Mutex���b�N���
	void EndMutexLock();
}