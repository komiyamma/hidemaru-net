/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the MIT License
 */

#include <windows.h>
#include "Shlobj.h "

// PathRemoveFileSpec �p 
#include <shlwapi.h> 
#pragma comment( lib, "shlwapi.lib" ) 

#include <vector>
#include <string>

#include "sendkeys.h"

using namespace std;

/*
pFiles:
		FileName�܂ł̃o�C�g�� 
pt:
		fNC�� TRUE�Ȃ�h���b�v�����Ƃ��̃J�[�\���ʒu�ł��B 
fNC:
		pt���L���Ȃ� TRUE���w�肵�܂��B 
fWide:
		FileName��wide characters�ō\�������Ȃ� TRUE���w�肵�܂��B 
FileName:
		�����̃t�@�C����

*/

const string dirname  = R"(C:\Program Files (x86)\Havok\HavokScriptTools\)";
const string filename = "HavokScriptDebugger.exe";
const string filefullpath = '"' + dirname + filename + '"';

// ���M����t�@�C���Q
vector<string> vStrBufFileNameList;
BOOL isExistHavokScriptDebugger = FALSE;


// �w��̃E�B���h�E�n���h����D&D���b�Z�[�W�𑗐M
void PostDragDropMessage(HWND hWnd) {
	if (hWnd) {
		// �t�@�C�����̋L�q�ɕK�v�ȃ������ʂ��v�Z
		int size=0;
		for(int i=0;i<(int)vStrBufFileNameList.size();i++) {
			size+=strlen((char *)vStrBufFileNameList[i].c_str())+1;
		}
		size++;
		HANDLE hMem=GlobalAlloc(GMEM_ZEROINIT,sizeof(DROPFILES)+size);
		char *p=(char *)GlobalLock(hMem);

		DROPFILES *DropFiles=(DROPFILES *)p;
		DropFiles->pFiles=sizeof(*DropFiles);
		DropFiles->pt.x=10;
		DropFiles->pt.y=10;
		DropFiles->fNC=1;
		DropFiles->fWide=0;
		p+=sizeof(*DropFiles);

		for(int i=0;i<(int)vStrBufFileNameList.size();i++) {
			strcpy(p,(char *)(vStrBufFileNameList[i]).c_str());
			p+=strlen(p)+1;
		}
		*p='\0';
		GlobalUnlock(hMem);
		::PostMessage(hWnd,WM_DROPFILES,(WPARAM)hMem,0);
	}
}

vector<HWND> subWindowList;
HWND hWindMain = NULL;

// �w��̐e�E�B���h�E�n���h���́A�q���̃E�B���h�E�S�Ăɑ΂��āA�ċA�I��PostDragDropMessage�֐������s����B
void IteratePostDragDropMessage(HWND hWndParent)
{
    HWND hWnd;

    char buff[MAX_PATH];
    ::GetWindowText(hWndParent, buff, sizeof(buff));

    hWnd = ::FindWindowEx(hWndParent, NULL, NULL, NULL);
    while (hWnd != NULL)
    {
        IteratePostDragDropMessage(hWnd); // �q��������A�ċA�łǂ�ǂ�
        hWnd = ::FindWindowEx(hWndParent, hWnd, NULL, NULL);
		subWindowList.push_back(hWnd);
		// printf("HWND=%08x, Caption=%s\r\n", hWndParent, buff);

		// �q���E�B���h�E�ɑ΂��ĥ��������ς�����D&D
		PostDragDropMessage(hWnd);
   }
}



// �g�b�v���x���̃E�B���h�E�̊e�X�̃E�B���h�E�n���h���ɑ΂��čs���֐�
BOOL CALLBACK EnumWindowsProc(HWND hWnd , LPARAM lp) {
	// �^�C�g������B

	// �^�C�g���������̂̓_���AHavokScript�Ƃ����^�C�g��������B
	char strWindowText[1024];
	::GetWindowText(hWnd , strWindowText , 1024);
	if (! strstr(strWindowText, "Havok Script Debugger") ) {
		return TRUE;
	}

	// ���̓N���X�B

	// Havok Script Debugger �Ƃ��������񂪂���
	char strClassName[1024];
	::GetClassName( hWnd, strClassName, sizeof(strClassName) ); // �Y���E�B���h�E�̃N���X���擾

	if ( strstr(strClassName, "HwndWrapper") && strstr(strClassName, "HavokScriptDebugger.exe;;") ) {
		hWindMain = hWnd;
		isExistHavokScriptDebugger = TRUE;
		IteratePostDragDropMessage(hWnd);
	}

	return TRUE;
}



// �w��̐e�E�B���h�E�n���h���́A�q���̃E�B���h�E�S�Ăɑ΂��āA�ċA�I��PostDragDropMessage�֐������s����B
void IsHavokScriptDebuggerStandByOKSub(HWND hWndParent)
{
    HWND hWnd;

    char buff[MAX_PATH];
    ::GetWindowText(hWndParent, buff, sizeof(buff));

    hWnd = ::FindWindowEx(hWndParent, NULL, NULL, NULL);
    while (hWnd != NULL)
    {
        IsHavokScriptDebuggerStandByOKSub(hWnd); // �q��������A�ċA�łǂ�ǂ�
        hWnd = ::FindWindowEx(hWndParent, hWnd, NULL, NULL);
		// printf("HWND=%08x, Caption=%s\r\n", hWndParent, buff);
		// �q���E�B���h�E�𑫂�����
		subWindowList.push_back(hWnd);
   }
}


// �g�b�v���x���̃E�B���h�E�̊e�X�̃E�B���h�E�n���h���ɑ΂��čs���֐�
BOOL CALLBACK IsHavokScriptDebuggerStandByOK(HWND hWnd , LPARAM lp) {
	// �^�C�g������B

	// �^�C�g���������̂̓_���AHavokScript�Ƃ����^�C�g��������B
	char strWindowText[1024];
	::GetWindowText(hWnd , strWindowText , 1024);
	if (! strstr(strWindowText, "Havok Script Debugger") ) {
		return TRUE;
	}

	// ���̓N���X�B

	// Havok Script Debugger �Ƃ��������񂪂���
	char strClassName[1024];
	::GetClassName( hWnd, strClassName, sizeof(strClassName) ); // �Y���E�B���h�E�̃N���X���擾
	if ( strstr(strClassName, "HwndWrapper") && strstr(strClassName, "HavokScriptDebugger.exe;;") ) {
		hWindMain = hWnd;
		isExistHavokScriptDebugger = TRUE;
		subWindowList.clear(); // ��[�N���A
		IsHavokScriptDebuggerStandByOKSub(hWnd);
	}

	return TRUE;
}


void SearchLaunched(char *szFilename, char *szLine, BOOL isLaunch) {
	if (strcmp(szLine, "0")==0) {
		szLine[0] = '1';
	}

	char buff[MAX_PATH]; 

	// ���s���v���O�����̃t���p�X 
	::GetModuleFileName(NULL, buff, MAX_PATH); 

	// �Ō�����폜���ăf�B���N�g�������𓾂� 
	::PathRemoveFileSpec( buff ); 

	string path_dir = buff;

	for (int i=0; i<20; i++) {
		Sleep(300);
		EnumWindows(IsHavokScriptDebuggerStandByOK, NULL);

		// �q�E�B���h�E�����낼��Ƃ���
		if ( subWindowList.size() >= 3 ) { 
			Sleep(400);

			string sendkeys = path_dir + "\\" + "sendkeys.exe";
			string ctrlg = sendkeys + " " + filefullpath +" [Ctrl]";
			system(ctrlg.c_str());
			ctrlg = sendkeys + " " + filefullpath + " [Ctrl]g";
			system(ctrlg.c_str());

			Sleep(300);
			ctrlg = sendkeys + " " + filefullpath + " [Ctrl]z";
			system(ctrlg.c_str());

			// ������
			HWND hWndGoToLine = NULL;
			for ( int i=0; i<10; i++) {
				Sleep(200);
				hWndGoToLine = FindWindow(NULL, "Go To Line");
				if ( hWndGoToLine) { break; }
			}

			if ( hWndGoToLine ) {
				Sleep(200);
				string line = sendkeys + " " + filefullpath +" "+string(szLine);
				system( line.c_str() );

				string enter = sendkeys + " " + filefullpath + " [ENTER]";
				system( enter.c_str() );

				if (isLaunch) {
					Sleep(500);
					string f6 = sendkeys + " " + filefullpath + " [F6]'";
					system(f6.c_str());

					for (int i = 0; i < 5; i++) {
						Sleep(100);
						if (FindWindow(NULL, "Havok Script Project Settings")) {
							break;
						}
					}
					string f6enter = sendkeys + " \"Havok Script Project Settings\" [ENTER]'";
					system(f6enter.c_str());
				}
				break;
			}
		}
	}

	exit(-1);
}



void main(int argc, char** argv) {

	if ( argc >= 3 ) { // �����L
		for ( int c=1; c<argc; c++) {
			vStrBufFileNameList.push_back(argv[c]); // �Ƃ肠�����A�P�̂ݎ󂯕t����B�v���O�������̂́A�����]���\�B
		}

		// �g�b�v���x���E�B���h�E�ɑ΂��āA�R�[���o�b�N�֐����s
		EnumWindows(EnumWindowsProc, NULL);


		// ScriptDebugger�����݂��Ȃ������B
		if ( !isExistHavokScriptDebugger ) {
			const char *szFileName = filename.c_str();
			ShellExecute(NULL, "open", szFileName, argv[1], NULL, SW_SHOWNORMAL);
			isExistHavokScriptDebugger = true;

			SearchLaunched(argv[1], argv[2], TRUE);
		} else {

			if (subWindowList.size() == 0 ) {
			}

			SearchLaunched(argv[1], argv[2], FALSE);
		}
	}
}