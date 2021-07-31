/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

// dllmain.cpp : DLL �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <string>
#include <tchar.h>

#include "Mutex.h"
#include "SharedExport.h"
#include "hidemaruexe_export.h"
#include "string_converter.h"
#include "hm_original_encode_mapfunc.h"

using namespace std;

#pragma comment (lib, "winmm.lib")


#pragma data_seg("HM_SHARED_OUTPUT_PANE_SEG")
HWND hCurHidemaruWndHandle = 0;
wchar_t szBufList[BUF_LINE][BUF_CHRS] = {};			// �O���v���O�������珑�����܂��o�b�t�@�BBUF_LINE�s�ŁA1�s������BUF_CHRS�����܂ŁB
int InputIndex = 0;								    // �O�����狤�L�������̏������݂ŁA�����Ԗڂ܂ŏ�������ł��邩�B�z�ŗ��p�����B
int OtputIndex = 0;						            // ��������f���o�����̂��A���Ԗڂ܂œf���o�����̂��B�z�ŗ��p�����B
int lstInputTimeGetTime = ::timeGetTime();          // ��ԍŌ�ɊO����������ɏ������܂ꂽ�����B
int lstOtputTimeGetTime = 0;					    // ��ԍŌ�ɊO����������ɏ������܂ꂽ�����B
DWORD curAutoUpdateTimeGetTime = ::timeGetTime();
DWORD preAutoUpdateTimeGetTime = ::timeGetTime();
#pragma data_seg()



CHidemaruExeExport HMEXE;



void OutputDebugStream(const wchar_t *format, ...) {
	wchar_t szBufDebug[4096] = L"";

	va_list arg;

	va_start(arg, format);
	_vstprintf_s(szBufDebug, format, arg);
	va_end(arg);

	OutputDebugString(szBufDebug);
}



HWND hWndOutputPane = NULL;
void OnHidemaruHandleUpdate(HWND hLocalHidemaruWndHandle) {

	if (hCurHidemaruWndHandle) {
		if (hHmOutputPaneDLL && pOutputFunc && pOutputGetWindowFunc) {
			// �Ώۂ̊J���Ă���t�@�C�������ω�����ƁA�Ώۂ̃A�E�g�v�b�g�E�B���h�E�n���h�����ω�����\��
			hWndOutputPane = pOutputGetWindowFunc(hLocalHidemaruWndHandle);
		}
	}
}



// �O������G�ۃn���h����ݒ肷��B��ɏG�ۃ}�N������̐ݒ肪�z�肳��Ă���B
extern "C" intptr_t _cdecl SetHidemaruHandle(HWND handle) {
	hCurHidemaruWndHandle = handle;
	// OutputDebugStream("%d\n", handle);
	return 1;
}

intptr_t iCearMilliSecond = -1;
// ���ȏ㏑�����݂����������ꍇ�ɁA���̏������ݎ��A�N���A���邩�ǂ����B
extern "C" int _cdecl SetWithClearTime(intptr_t iMilliSecond) {
	iCearMilliSecond = iMilliSecond;
	return 1;
}

extern "C" void _stdcall SetSharedMessage(const char *szmsg) {
	SetSharedMessageA(szmsg);
}

// �O�����烁�b�Z�[�W��DLL�����ɃR�s�[����B
extern "C" void _stdcall SetSharedMessageA(const char *szmsg) {

	if (!szmsg) { return; }

	wstring wstr = cp932_to_utf16(szmsg);
	SetSharedMessageW(wstr.data());
}

// �O�����烁�b�Z�[�W��DLL�����ɃR�s�[����B
extern "C" void _stdcall SetSharedMessageW(const wchar_t *szmsg) {

	if (!szmsg) { return; }

	wstring wstr = wstring(szmsg);

	// �r��Mutex�I�u�W�F�N�g���쐬�B�n���h���𓾂� 
	Mutex::hMutex = CreateMutex(NULL, FALSE, Mutex::GetMutexLabel());
	Mutex::BgnMutexLock();

	// �o�b�t�@�͏��ԂɎg���Ă������ƂƂȂ�B
	// ���L�������ɃR�s�[����B
	_tcsncpy_s(szBufList[InputIndex], wstr.data(), BUF_CHRS - 1);
	// ���L�������̂ǂ̃C���f�b�N�X�܂Ŏg���������o���Ă����BBUF_LINE�܂ł����J��ŗ��p����B
	InputIndex++;
	if (InputIndex == BUF_LINE) { InputIndex = 0; }

	// ��ԍŌ�ɊO����������ւƏ������܂ꂽ���Ԃ��T���Ă���
	lstInputTimeGetTime = ::timeGetTime();

	Mutex::EndMutexLock();
}


DWORD curTimeGetTime = ::timeGetTime();
DWORD preTimeGetTime = ::timeGetTime();



// �O������G�ۃn���h����ݒ肷��B
bool isMustBreakLoop = 0;
unsigned __stdcall OutputSharedMessage(void *) {
	intptr_t ClearTime = iCearMilliSecond; // �_�b �Ō�̕\������30�b�ȏ�o�߂��Ă����ꍇ�A���̕�����\���̑O�ɁuOutput�g�̃N���A�v�������

	while (TRUE) {
		if (isMustBreakLoop) {
			break;
		}
		// �G�ۂ̃A�E�g�v�b�g�g�̑��x���l������ƁA���̒��x�͋x�e������ł��Ă悢�B
		Sleep(30);

		if (isMustBreakLoop) {
			break;
		}
		// �G�ۂ̃A�E�g�v�b�g�g�̑��x���l������ƁA���̒��x�͋x�e������ł��Ă悢�B
		Sleep(30);

		if (isMustBreakLoop) {
			break;
		}
		// �G�ۂ̃A�E�g�v�b�g�g�̑��x���l������ƁA���̒��x�͋x�e������ł��Ă悢�B
		Sleep(30);

		if (!IsWindow(hCurHidemaruWndHandle)) {
			hCurHidemaruWndHandle = CHidemaruExeExport::GetCurWndHidemaru();
		}

		// �G�ۂ̃E�B���h�E�n���h�����}�N�����������Ă��Ȃ��B�������Ȃ�
		if (!hCurHidemaruWndHandle) {
			continue;
		}

		/*
		���ڎ��s������@�͖����ł����A���X������ł͂���܂����A
		[�}�N��]��[�}�N���o�^]�œo�^�����}�N���̔ԍ����w�肵�āAWM_COMMAND�𑗂�Ƃ������Ƃ�
		�ł��Ă��܂��Ǝv���܂��B
		�Q�l�܂ł̏��Ƃ��āAwParam�Ɏw�肷��R�}���h�l�́A
		�}�N��1�`15�̃R�}���h�l��146�`160
		�}�N��16�`30�̃R�}���h�l��205�`219
		�}�N��31�`�̃R�}���h�l��234�`
		�ɂȂ��Ă��܂��B
		*/
		//	SendMessage( (HWND)hCurHidemaruWndHandle, 0x111, 235, 0 ); // 235=�}�N��32�̂���!!
		if (isMustBreakLoop) {
			break;
		}

		if (InputIndex == OtputIndex) {
			continue;
		}

		curTimeGetTime = ::timeGetTime();


		// ���[�J���̃n���h���Ƃ��āA�X�V����B
		HWND hLocalHidemaruWndHandle = hCurHidemaruWndHandle;
		OnHidemaruHandleUpdate(hLocalHidemaruWndHandle);

		// �Ō�ɏo�͂������Ԃ���A���b�ȏ�o�߂��Ă���B=�A�E�g�v�b�g�̈�͈�x�N���A���ׂ����B
		if (ClearTime > 0) {
			if ((int)curTimeGetTime - lstOtputTimeGetTime > ClearTime) {
				SendMessage((HWND)hWndOutputPane, 0x111, 1009, 0);  // �O��̌��ʂ͎c���Ȃ�
			}
		}

		if (isMustBreakLoop) {
			break;
		}

		// �r��Mutex�I�u�W�F�N�g���쐬�B�n���h���𓾂� 
		Mutex::hMutex = CreateMutex(NULL, FALSE, Mutex::GetMutexLabel());
		Mutex::BgnMutexLock();

		// �i���Ƀ��[�v���Ă��܂�Ȃ��悤�ɁA�ő�ł��ABUF_LINE-1�܂łň�x�o�͂���B
		int maxcnt = 0;
		while (TRUE) {

			if (isMustBreakLoop) {
				break;
			}

			wstring line = wstring(szBufList[OtputIndex]);

			if (pOutputFunc) {
				line += L"\n";

				BOOL success = FALSE;
				if (pOutputWFunc) {
					success = pOutputWFunc((HWND)hLocalHidemaruWndHandle, (wchar_t*)(line.data()));
				} else {
					vector<BYTE> bite_data = EncodeWStringToOriginalEncodeVector(line);
					// --------------------- �P�s�̕�����Ƀp�b�`�𓖂ĂĂ��� �����܂�----------------------------
					success = pOutputFunc((HWND)hLocalHidemaruWndHandle, bite_data.data());
				}

				// �A�E�g�v�b�g�p�l���ւ̏o�͂́A�l�܂��Ă�Ǝ��s���邱�Ƃ�����
				if (success) {

					// �������ď��߂āc
					szBufList[OtputIndex][0] = NULL;
					// �o�͂�����A���L�������̏o�̓C���f�b�N�X���P���₷�B
					// �P���₷
					OtputIndex++;
				}
			}

			// �r���Ō��݂̃A�E�g�v�b�g���Output�̊֐����ߑ��ł��Ȃ��Ƃ�������
			else {
				if (isMustBreakLoop) {
					break;
				}
				Sleep(30);
				if (isMustBreakLoop) {
					break;
				}
				Sleep(30);
				if (isMustBreakLoop) {
					break;
				}
				Sleep(30);
				continue;
			}

			// ���L�������̂ǂ̃C���f�b�N�X�܂ŏo�͂��������o���Ă����BBUF_LINE�܂ł����J��ŗ��p����B

			if (OtputIndex == BUF_LINE) { OtputIndex = 0; }
			// ���̓C���f�b�N�X�ƈ�v������A�S�����������܂ŁB

			if (InputIndex == OtputIndex) {
				break;
			}

			// �ő�ł��ABUF_LINE-1�܂łň�x�o�͂���B
			maxcnt++;
			if (maxcnt == BUF_LINE - 1) {
				break;
			}
		}

		lstOtputTimeGetTime = curTimeGetTime;
		Mutex::EndMutexLock();

		Sleep(10);

		if (isMustBreakLoop) {
			break;
		}

	}

	return 0;
}


