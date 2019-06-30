/*
 * Copyright(C) 2017 Akitsugu Komiyama
 * under the MIT License
 */ 


#include <windows.h>
#include <process.h>
#include <tchar.h>
#include <string>

#include "HmStatusBarPreviewFormController.h"
#include "HmStatusBarText.h"
#include "HmEditClient.h"
#include "HmCursor.h"
#include "OutputDebugStream.h"
#include "hidemaruexe_export.h"


using namespace std;

CHidemaruExeExport HMEXE;


HWND hmWndHandle = NULL;

BOOL CALLBACK EnumChildProc(HWND hWnd, LPARAM lParam)
{
	wchar_t szTitle[1024];
	char szClass[1024];

	GetWindowText(hWnd, szTitle, _countof(szTitle));    // �L���v�V�����̎擾
	GetClassNameA(hWnd, szClass, _countof(szClass));    // �N���X������̎擾 W�͖����̂Œ���

	// �G�ۃG�f�B�^���̃X�e�[�^�X�o�[�E�B���h�E�ɑ΂��āc
	if (strcmp(szClass, "msctls_statusbar32") == 0) {

		HmStatusBarText::OnEnumChildProc(hWnd);
	}

	// �G�ۃG�f�B�^���̕ҏW�̈�ɑ΂��āc
	else if (strcmp(szClass, "HM32CLIENT") == 0) {

		// �}�E�X�����̒��ɂȂ���΁c
		if (!HidemaruEditClient::IsInMouse(hWnd)) {
			HmStatsusBarPreviewFormController::Destroy();
		}

	}

	return TRUE;
}


// �����ꂩ�̏G�ۂ��A�N�e�B�u
bool IsHidemaruActive() {
	HWND hWnd = GetForegroundWindow();
	char name[256] = "";
	GetClassNameA(hWnd, name, _countof(name));

	// �f�X�N�g�b�v�ł��������́A�X�g�A�A�v����
	return strcmp(name, "Hidemaru32Class") == 0 || strcmp(name, "Hidemaru32Class_Appx") == 0;
}


const int iThreadInterval = 200;
extern HWND hCurrentForeHidemaruHandle;

// Tic�X���b�h�֐�
bool beExitThreadCheckStatusBar = false; // �X���b�h���甲����ׂ�
unsigned __stdcall ThreadCheckStatusBar(void *p)
{
	while(TRUE) {
		if (beExitThreadCheckStatusBar) {
			// OutputDebugStream( L"BREAK\n" );
			break;
		}

		// �}�E�X�̌`���n���h���ǂ���
		bool isGlobalCursurIsHand = IsGlobalCursorIsHand();
		// �n���h����Ȃ����������
		if (!isGlobalCursurIsHand) {

			// OutputDebugStream(L"�Ⴄ�������");
			HmStatsusBarPreviewFormController::Destroy();

			Sleep(iThreadInterval);
			continue;
		}

		// �A�N�e�B�u�E�B���h�E�͏G�ۂ��ǂ���
		bool isHidemaruActive = IsHidemaruActive();
		if (!isHidemaruActive) {
			// OutputDebugStream(L"�Ⴄ�������");
			HmStatsusBarPreviewFormController::Destroy();

			Sleep(iThreadInterval);
			continue;

		}

		/*
		// �Ȃ����E�B���h�E���̂��̂ł͂Ȃ��Ȃ��Ă���
		if (!IsWindow(hCurrentForeHidemaruHandle)) {
			hCurrentForeHidemaruHandle = CHidemaruExeExport::GetCurWndHidemaru();
		}
		*/

		// �G�ۂ̃n���h���̓t�H�A�O���E���h�ł͂Ȃ��B
		if (hmWndHandle != hCurrentForeHidemaruHandle ) {
			// OutputDebugStream(L"�Ⴄ�������");
			HmStatsusBarPreviewFormController::Destroy();

			Sleep(iThreadInterval);
			continue;
		}

		// �G�ۂ̎q�E�B���h�E�𖈉�T���B
		EnumChildWindows(hmWndHandle, EnumChildProc, (LPARAM)NULL);

		Sleep(iThreadInterval / 20); // �����܂ŗ����玟����X���[�Y
		continue;


	}
	return 0;
}

HANDLE hThread = NULL;


extern "C" __declspec(dllexport) intptr_t SetHidemaruHandle(HWND pHmWndHandle) {
	// �G�ۂ̃n���h�������B���̂܂܋��p�������ɂ��i�[���Ă����Bintptr_t�l�P�Ȃ̂ŁA����Mutex���Ȃ��đ��v���낤�B
	hCurrentForeHidemaruHandle = hmWndHandle = pHmWndHandle;

	if (hThread == NULL) {
		// �X���b�h�̍쐬
		hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadCheckStatusBar, NULL, 0, NULL);
	}
	return TRUE;
}

int HmBColor = 0xF5F5F5; // �f�t�H���g�̓z���C�g�X���[�N
extern "C" __declspec(dllexport) intptr_t SetHidemaruStrParam(wchar_t* param_name, wchar_t* param_value) {
	return TRUE;
}

extern "C" __declspec(dllexport) intptr_t SetHidemaruNumParam(wchar_t* param_name, intptr_t param_value) {
	if (wstring(param_name) == L"bcolor") {
		try {
			HmStatsusBarPreviewFormController::SetBColor((int)param_value);
		}
		catch (exception e) {
			OutputDebugStringA(e.what());
			return FALSE;
		}
	}
	return TRUE;
}


extern "C" __declspec(dllexport) intptr_t DllDetachFunc_After_Hm866() {
	// �I������ׂ��t���O�𗧂Ă�
	beExitThreadCheckStatusBar = true;
	WaitForSingleObject(hThread, iThreadInterval * 2); // �ň��ł�Tic�̂Q�{�܂�
	// �X���b�h�̏I��
	CloseHandle(hThread);

	hThread = NULL;

	// �t�H�[��������
	HmStatsusBarPreviewFormController::Destroy();

	// �X���b�h�̃X�g�b�v
	return TRUE;
}

