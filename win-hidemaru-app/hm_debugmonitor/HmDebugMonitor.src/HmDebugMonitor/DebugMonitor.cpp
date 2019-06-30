/*
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the MIT License
 */


#define _CRT_SECURE_NO_WARNINGS

/*++

OutputDebugString�̕��������o���B
32bit/64bit�ǂ���ł��R���p�C���\�Ȃ悤�ɂ��Ă���B(�\�����[�V������v���W�F�N�g�̐ݒ�I�ɂ�)
�E�^�[�Q�b�g�̃v���Z�X�̢�N���X������킩���Ă���Ȃ�΁A�r�b�g���̕ǂ��z������B
�E�^�[�Q�b�g�̃t�@�C���������������Ă��Ȃ��Ȃ�΁A�r�b�g���̕ǂ͒������Ȃ��B
�@���̏ꍇ�A�^�[�Q�b�g�̃r�b�g��(32 or 64)�Ɠ����r�b�g���ŃR���p�C�����Ă����K�v������B
 �Ȃ��AOutputDebugString���̂̎擾�́A64bit��32bit���݂ł����Ȃ�����Ă����B

 --*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <psapi.h>  // Psapi.Lib
#include <mmsystem.h>
#include <string>
#include <fstream>
#include <vector>
#include <map>

#include "shlwapi.h"

#include "resource.h"
#include "utf8_to_sjis.h"
#include "utf16_to_sjis.h"
#include "TextBox.h"
#include "RegexBox.h"
#include "HmDebugMonitorForm.h"
#include "RecieveButton.h"
#include "OnigRegex.h"
#include "nmh.h"

using namespace std;

#pragma comment (lib, "winmm.lib")
#pragma comment(lib,"psapi.lib")


// �ȉ��^�[�Q�b�g�̃N���X�����A�v���Z�X�l�[��(�̈ꕔ)�̂ǂ��炩�킩���Ă�΂Ȃ�Ƃ��Ȃ�B
#define TARGET_CLASS_WND "Hidemaru32Class"
#define TARGET_CLASS_APP_WND "Hidemaru32Class_Appx"


HMODULE hModuleHMJRE = NULL;
typedef void (_cdecl *PFNSETMESSAGE)(char *);
PFNSETMESSAGE pSetSharedMessage = NULL;
char szBufSendMessage[512] = "";
char szDebugMonErrorMsg[256] = "";



int curTimeGetTime = ::timeGetTime();
int preTimeGetTime = ::timeGetTime();
int lstIntrGetTime = ::timeGetTime();
int minPrintGetTime = ::timeGetTime();

map<string, int> mapstr; // �p���I�Ƀ��O���o�Ă���ꍇ�̃o�b�t�@;


HANDLE AckEvent;
HANDLE ReadyEvent;
HANDLE SharedFile;
LPVOID SharedMem;
LPSTR  String;
DWORD  ret;
DWORD  LastPid;
LPDWORD pThisPid;
BOOL    DidCR;

SECURITY_ATTRIBUTES sa;
SECURITY_DESCRIPTOR sd;

BOOL isDoingRecieveFlag = TRUE;

int iHidemaruErrorCnt = 0;

static BOOL iPrevIsHidemaruStart = FALSE;
// ���C���֐�
void InitDebugMonitor(HWND hEdit) {


	//--------------- �Z�L�����e�B
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = &sd;

	if (!InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION)) {
		sprintf(szDebugMonErrorMsg, "�G�ۃf�o�b�K�[:InitializeSecurityDescriptor���邱�Ƃ��o���܂���B�G���[�ԍ� == %d\n",
			GetLastError());
		MessageBox(NULL, szDebugMonErrorMsg, szDebugMonErrorMsg, NULL);
		exit(1);
	}

	if(!SetSecurityDescriptorDacl(&sd, TRUE, (PACL)NULL, FALSE)) {
		sprintf(szDebugMonErrorMsg, "�G�ۃf�o�b�K�[:SetSecurityDescriptorDacl���邱�Ƃ��o���܂���B�G���[�ԍ� == %d\n",
			GetLastError());
		MessageBox(NULL, szDebugMonErrorMsg, szDebugMonErrorMsg, NULL);
		exit(1);
	}

	// OutputDebugString�ŏ������񂾂�[�Ƃ����C�x���g���擾���邽�߂ɃC�x���g�쐬�B
	//--------------- ACK�C�x���g(�V�O�i����ԂŁA����OutputDebugString()�p��)
	AckEvent = CreateEvent(&sa, FALSE, FALSE, "DBWIN_BUFFER_READY");

	if (!AckEvent) {
		sprintf(szDebugMonErrorMsg, "�G�ۃf�o�b�K�[:�����I�u�W�F�N�g�𐶐����邱�Ƃ��o���܂���B�G���[�ԍ� == %d\n",
			GetLastError());
		MessageBox(NULL, szDebugMonErrorMsg, szDebugMonErrorMsg, NULL);
		exit(1);
	}

	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		sprintf(szDebugMonErrorMsg, "�G�ۃf�o�b�K�[: ���ŕʂ̃f�o�b�O���j�^�[�n��̃A�v���P�[�V���������s����Ă��܂��B\n");
	//	MessageBox(NULL, szDebugMonErrorMsg, szDebugMonErrorMsg, NULL);
	//	exit(1);
	}

	//--------------- READY�C�x���g(�V�O�i����Ԃ�OutputDebugString()�o�͂�����)
	ReadyEvent = CreateEvent(&sa, FALSE, FALSE, "DBWIN_DATA_READY");

	if (!ReadyEvent) {
		sprintf(szDebugMonErrorMsg, "�G�ۃf�o�b�K�[:�����I�u�W�F�N�g�𐶐����邱�Ƃ��o���܂���B�G���[�ԍ� == %d\n",
			GetLastError());
		MessageBox(NULL, szDebugMonErrorMsg, szDebugMonErrorMsg, NULL);
		exit(1);
	}

	// OutputDebugString()�ŏ������܂��o�b�t�@�����L�������ŊJ��
	//--------------- "DBWIN_BUFFER"�Ƃ������̂̋��L������(4096bytes)
	SharedFile = CreateFileMapping(
		(HANDLE)-1,
		&sa,
		PAGE_READWRITE,
		0,
		4096,
		"DBWIN_BUFFER");

	if (!SharedFile) {
		sprintf(szDebugMonErrorMsg, "�G�ۃf�o�b�K�[:�t�@�C���}�b�p�[�I�u�W�F�N�g�𐶐����邱�Ƃ��o���܂���B�G���[�ԍ� == %d\n",
			GetLastError());
		MessageBox(NULL, szDebugMonErrorMsg, szDebugMonErrorMsg, NULL);
		exit(1);
	}

	SharedMem = MapViewOfFile(
		SharedFile,
		FILE_MAP_READ,
		0,
		0,
		512);

	if (!SharedMem) {
		sprintf(szDebugMonErrorMsg, "�G�ۃf�o�b�K�[:���p�������I�u�W�F�N�g�𐶐����邱�Ƃ��o���܂���B�G���[�ԍ� == %d\n", 
			GetLastError());
		MessageBox(NULL, szDebugMonErrorMsg, szDebugMonErrorMsg, NULL);
		exit(1);
	}

	//--------------- �擪DWORD���v���Z�XID�A�ȉ����i�[������
	String = (LPSTR)SharedMem + sizeof(DWORD);
	pThisPid = (LPDWORD)SharedMem;

	LastPid = 0xffffffff;
	DidCR = TRUE;

	// ���������܂ő҂�
	SetEvent(AckEvent);
}

//  �^�[�Q�b�g�̃v���Z�XID���A�G�ۈȉ���tooltips_class32�Ƃ����E�B���h�E�n���h����
static bool IsTargetProcessIsHidemaruToolTipsWindow(const DWORD dwPID)	// �v���Z�XID
{
	HWND hWnd = FindWindow(TARGET_CLASS_WND, NULL);

	// ����������X�g�A�A�v���ł�
	if (!hWnd)  { hWnd = FindWindow(TARGET_CLASS_APP_WND, NULL); }

	// ��������
	if (!hWnd) {
		return false;
	}

	// �v���Z�X�n���h�����擾
	HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
	if (!hProcess) {
		return false;
	}

	// ���s�\�t�@�C�������擾
	char szTargetImage[MAX_PATH];
	DWORD dwCopiedLength = ::GetProcessImageFileName(hProcess, szTargetImage, _countof(szTargetImage));

	char szExeFullPath[MAX_PATH];
	GetModuleFileName( NULL, szExeFullPath, _countof(szExeFullPath) );

	_strlwr_s(szTargetImage);

	if (strstr(szTargetImage, "hidemaru.exe")) {
		return true;
	}

	return false;
}

int isSjisLineEnd = 0;
string sjisLineStock = "";
// �X���b�h
unsigned __stdcall ThreadExternalProcDebugMonitor(void *lpx) {

	for (;;) {

		ret = WaitForSingleObject(ReadyEvent, INFINITE);

		// -----------���łɑ��݂���ꍇ�̏���
		// �E�B���h�E�^�C�v�̏ꍇ���l���āA�A�C�R��������Ă�����A���ւƖ߂��B
		HWND hWndDebugMonitor = FindWindow(szClassName, NULL);
		SetWindowPos(hWndDebugMonitor, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);


		if (ret != WAIT_OBJECT_0) {

			sprintf(szDebugMonErrorMsg, "�G�ۃf�o�b�K�[: �ҋ@�G���[�B�G���[�ԍ� == %dn",
				GetLastError());
			MessageBox(NULL, szDebugMonErrorMsg, szDebugMonErrorMsg, NULL);
			exit(1);

		} else {
			// �ȉ��󂯎������������ǂ��������鏈��

			if (LastPid != *pThisPid) {

				LastPid = *pThisPid;
				if (!DidCR) {
					// putchar('\n');
					DidCR = TRUE;
				}
			}

			// �^�[�Q�b�g��ProcessID�̂��̂����\������B
			// ���x���x�`�F�b�N����K�v������B

			// �^�[�Q�b�g�̃v���Z�X�h�c
			bool hWndTargetProcessIsHidemaruToolTipsWindow = IsTargetProcessIsHidemaruToolTipsWindow(*pThisPid);

			// printf("%s", String); // utf8
			string sjis = String;

			if (sjis == ("xxxxxxxxxx\n")) {
				// ��M���~���Ă��Ȃ��B���O�ɏG�ۂ��N�������ƌ����ĂȂ��B
				if (iPrevIsHidemaruStart == FALSE) {
					WriteRichEdit(hEdit, "�G�ۂ̍ċN����F���B\n\n");
					iPrevIsHidemaruStart = TRUE;
					hWndTargetProcessIsHidemaruToolTipsWindow = false;
					AllRecievedLog.push_back("�G�ۂ̍ċN����F���B\n\n");
				}
				else {
					hWndTargetProcessIsHidemaruToolTipsWindow = false;
				}
			}
			else {
				iPrevIsHidemaruStart = FALSE;
			}

			// ����X�g�b�N
			sjisLineStock += sjis;

			// ���s�����݂��Ȃ�
			if (sjis.find("\n") == string::npos) {
				// �s�����܂�ł��Ȃ�
				isSjisLineEnd = 0;
			}
			// �s�����܂�
			else {
				// �X�g�b�N��������n��
				sjis = sjisLineStock;
				// �n�����̂ŃX�g�b�N�̓N���A
				sjisLineStock = "";
				// �s�����[�h
				isSjisLineEnd = 1;
			}

			string from = "showvars";
			string::size_type pos = sjis.find(from);
			if (pos != string::npos) {
				sjis.replace(pos, from.size(), "�ϐ��ꗗ");
			}

			//����̓t�@�C���̕ۑ���ǂݍ��݂łȂ����o��
			if (sjis.find("shell\\comdlg32\\fileopensave.cpp(") != string::npos) {
				hWndTargetProcessIsHidemaruToolTipsWindow = false;
			}

			if (isSjisLineEnd) {
				// utf8�n�Ƃ������肪�o����Asjis��
				enum NMH_CHAR_CODE n = nmh_code_detect(sjis.c_str(), strlen(sjis.c_str()));
				const char *szname = NMH_CHAR_CODE_NAME_LIST[n];
				if (strstr(szname, "UTF8")) {
					sjis = utf8_to_cp932(sjis);
				}
			}

			// �^�[�Q�b�g�v���Z�X�������オ���Ă��鎞�ɂ́A�^�[�Q�b�g�v���Z�X�ȊO�̂��̂͏o�͂��Ȃ��悤�ɂ���(�ז��ɂȂ邩��)
			if (isSjisLineEnd && isDoingRecieveFlag && hWndTargetProcessIsHidemaruToolTipsWindow) {
				
				if (DidCR) {
					printf("\n------------------------------------------------ �v���Z�XID:%3u:����̎�M\n", LastPid);
				}

				int modePrint = 1;

				// 800�~���b�C���^���v�g������΁A�p���͂��Ă��Ȃ��ƍl����B
				if ( int(::timeGetTime() - preTimeGetTime) > 800 ) {
					// �p�����������A
					lstIntrGetTime = ::timeGetTime();
					// �o�^���Ă����}�b�v���N���A
					mapstr.clear();
				}

				// �W�b�Ԉȏ㑱���Ă���Ȃ�΁A���̊Ԃɂ����������͕\�����Ȃ��B
				if ( int(::timeGetTime() - lstIntrGetTime) > 8000 ) {
					// ���łɓo�^���Ă�������o�͂��Ȃ��B
					if ( mapstr[sjis] ) {
						modePrint = 0;
					}
				}

				// �R�b�Ԃő����Ă���B
				if ( int(::timeGetTime() - lstIntrGetTime) > 3000 ) {

					// �n�b�V���̃J�E���^���Z�B
					mapstr[sjis] += 1;
				}

				// �P�O�Ƀ��b�Z�[�W��f���o�������ԂƂ��Č��݂̎��Ԃ�����
				preTimeGetTime = ::timeGetTime();

				// �O��ȗ��ƃv�����g���Ă���Q�b�o��
				// �ȗ��p�^�[�����o�����߁AmodePrint2�Ƃ���B
				if ( modePrint == 0 && int(::timeGetTime() - minPrintGetTime) > 2000 ) {
					minPrintGetTime = ::timeGetTime();
					modePrint = 2;
				}

				// �o�͂��ׂ����[�h
				if ( modePrint ) {

					// �O���[�o���ɃR�s�[
					strncpy( szBufSendMessage, sjis.c_str(), sizeof(szBufSendMessage) );
					szBufSendMessage[sizeof(szBufSendMessage)-1] = NULL;

					// ���łɃT�C�Y���f�J������ꍇ�A����������B
					// OVER_ALL_RECIEVED_LOG_SIZE�s�𒴂���
					if ( AllRecievedLog.size() > OVER_ALL_RECIEVED_LOG_SIZE ) {
						// �Ō�100�s���R�s�[
						vector<string> tmp;
						for ( int i=int(AllRecievedLog.size()-JUST_ALL_RECIEVED_LOG_SIZE); i < int(AllRecievedLog.size()); i++ ) {
							tmp.push_back( AllRecievedLog[i] );
						}
						// 100�s���Ƀf�[�^��������
						AllRecievedLog = tmp;
						// 100�s���ŕ\�����Ȃ���
						ReWriteAllRichEdit(hEdit);
					}

					if ( strstr(szBufSendMessage, "HIDEMARUERR: " ) ) {
						iHidemaruErrorCnt++;
					}

					// �ʏ�̃v�����g
					if ( modePrint == 1 ) {
						// printf("%s", sjis.c_str() );
						AllRecievedLog.push_back( szBufSendMessage );

						WriteRichEdit(hEdit, szBufSendMessage );

						// �ȗ����[�h
					} else if ( modePrint == 2) {
						// printf("�G��:�`�������O�̌J��Ԃ��Ȃ��ߏȗ����`\n");
						AllRecievedLog.push_back(  "�`�������O�̌J��Ԃ��Ȃ��ߏȗ����`\n" );
						WriteRichEdit(hEdit, "�`�������O�̌J��Ԃ��Ȃ��ߏȗ����`\n");

						// �v�f���o�͂���
						map<string, int>::iterator it = mapstr.begin();
						while( it != mapstr.end() )
						{
							string s = it->first;
							if ( mapstr[s] > 0 ) {
								// printf("%s", s.c_str() );
								AllRecievedLog.push_back( szBufSendMessage );
								WriteRichEdit(hEdit, szBufSendMessage );

								mapstr[s]--;
								if ( mapstr[s] < 0 ) { mapstr[s] = 0; }
							}
							++it;
						}
					}


					if (iHidemaruErrorCnt > 10) {
						AllRecievedLog.push_back(  "\n�G��: �G���[�������Ȃ��Ă��܂��B��M���~���܂��B\n" );
						WriteRichEdit(hEdit, "\n�G��: �G���[�������Ȃ��Ă��܂��B��M���~���܂��B\n");

						// ��M���~
						if ( isDoingRecieveFlag ) {
							SwitchRecieveStatus();
						}
					}

				}
			}

		}
		DidCR = (*String && (String[strlen(String) - 1] == '\n'));

		SetEvent(AckEvent);

	}

	return 0;
}

