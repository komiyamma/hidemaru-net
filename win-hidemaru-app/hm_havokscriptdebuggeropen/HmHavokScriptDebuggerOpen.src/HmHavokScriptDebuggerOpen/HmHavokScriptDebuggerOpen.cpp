
#include <windows.h>
#include "Shlobj.h "

// PathRemoveFileSpec 用 
#include <shlwapi.h> 
#pragma comment( lib, "shlwapi.lib" ) 

#include <vector>
#include <string>

#include "sendkeys.h"

using namespace std;

/*
pFiles:
		FileNameまでのバイト数 
pt:
		fNCが TRUEならドロップしたときのカーソル位置です。 
fNC:
		ptが有効なら TRUEを指定します。 
fWide:
		FileNameがwide charactersで構成されるなら TRUEを指定します。 
FileName:
		複数のファイル名

*/

const string dirname  = R"(C:\Program Files (x86)\Havok\HavokScriptTools\)";
const string filename = "HavokScriptDebugger.exe";
const string filefullpath = '"' + dirname + filename + '"';

// 送信するファイル群
vector<string> vStrBufFileNameList;
BOOL isExistHavokScriptDebugger = FALSE;


// 指定のウィンドウハンドルにD&Dメッセージを送信
void PostDragDropMessage(HWND hWnd) {
	if (hWnd) {
		// ファイル名の記述に必要なメモリ量を計算
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

// 指定の親ウィンドウハンドルの、子供のウィンドウ全てに対して、再帰的にPostDragDropMessage関数を実行する。
void IteratePostDragDropMessage(HWND hWndParent)
{
    HWND hWnd;

    char buff[MAX_PATH];
    ::GetWindowText(hWndParent, buff, sizeof(buff));

    hWnd = ::FindWindowEx(hWndParent, NULL, NULL, NULL);
    while (hWnd != NULL)
    {
        IteratePostDragDropMessage(hWnd); // 子供いたら、再帰でどんどん
        hWnd = ::FindWindowEx(hWndParent, hWnd, NULL, NULL);
		subWindowList.push_back(hWnd);
		// printf("HWND=%08x, Caption=%s\r\n", hWndParent, buff);

		// 子供ウィンドウに対して･･かたっぱしからD&D
		PostDragDropMessage(hWnd);
   }
}



// トップレベルのウィンドウの各々のウィンドウハンドルに対して行う関数
BOOL CALLBACK EnumWindowsProc(HWND hWnd , LPARAM lp) {
	// タイトルから。

	// タイトルが無いのはダメ、HavokScriptというタイトルがある。
	char strWindowText[1024];
	::GetWindowText(hWnd , strWindowText , 1024);
	if (! strstr(strWindowText, "Havok Script Debugger") ) {
		return TRUE;
	}

	// 次はクラス。

	// Havok Script Debugger という文字列がある
	char strClassName[1024];
	::GetClassName( hWnd, strClassName, sizeof(strClassName) ); // 該当ウィンドウのクラス名取得

	if ( strstr(strClassName, "HwndWrapper") && strstr(strClassName, "HavokScriptDebugger.exe;;") ) {
		hWindMain = hWnd;
		isExistHavokScriptDebugger = TRUE;
		IteratePostDragDropMessage(hWnd);
	}

	return TRUE;
}



// 指定の親ウィンドウハンドルの、子供のウィンドウ全てに対して、再帰的にPostDragDropMessage関数を実行する。
void IsHavokScriptDebuggerStandByOKSub(HWND hWndParent)
{
    HWND hWnd;

    char buff[MAX_PATH];
    ::GetWindowText(hWndParent, buff, sizeof(buff));

    hWnd = ::FindWindowEx(hWndParent, NULL, NULL, NULL);
    while (hWnd != NULL)
    {
        IsHavokScriptDebuggerStandByOKSub(hWnd); // 子供いたら、再帰でどんどん
        hWnd = ::FindWindowEx(hWndParent, hWnd, NULL, NULL);
		// printf("HWND=%08x, Caption=%s\r\n", hWndParent, buff);
		// 子供ウィンドウを足しこむ
		subWindowList.push_back(hWnd);
   }
}


// トップレベルのウィンドウの各々のウィンドウハンドルに対して行う関数
BOOL CALLBACK IsHavokScriptDebuggerStandByOK(HWND hWnd , LPARAM lp) {
	// タイトルから。

	// タイトルが無いのはダメ、HavokScriptというタイトルがある。
	char strWindowText[1024];
	::GetWindowText(hWnd , strWindowText , 1024);
	if (! strstr(strWindowText, "Havok Script Debugger") ) {
		return TRUE;
	}

	// 次はクラス。

	// Havok Script Debugger という文字列がある
	char strClassName[1024];
	::GetClassName( hWnd, strClassName, sizeof(strClassName) ); // 該当ウィンドウのクラス名取得
	if ( strstr(strClassName, "HwndWrapper") && strstr(strClassName, "HavokScriptDebugger.exe;;") ) {
		hWindMain = hWnd;
		isExistHavokScriptDebugger = TRUE;
		subWindowList.clear(); // 一端クリア
		IsHavokScriptDebuggerStandByOKSub(hWnd);
	}

	return TRUE;
}


void SearchLaunched(char *szFilename, char *szLine, BOOL isLaunch) {
	if (strcmp(szLine, "0")==0) {
		szLine[0] = '1';
	}

	char buff[MAX_PATH]; 

	// 実行中プログラムのフルパス 
	::GetModuleFileName(NULL, buff, MAX_PATH); 

	// 最後尾を削除してディレクトリ部分を得る 
	::PathRemoveFileSpec( buff ); 

	string path_dir = buff;

	for (int i=0; i<20; i++) {
		Sleep(300);
		EnumWindows(IsHavokScriptDebuggerStandByOK, NULL);

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

			// 検索窓
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

	if ( argc >= 3 ) { // 引数有
		for ( int c=1; c<argc; c++) {
			vStrBufFileNameList.push_back(argv[c]); // とりあえず、１つのみ受け付ける。プログラム自体は、複数転送可能。
		}

		// トップレベルウィンドウに対して、コールバック関数実行
		EnumWindows(EnumWindowsProc, NULL);


		// ScriptDebuggerが存在しなかった。
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