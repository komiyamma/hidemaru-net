#define _CRT_SECURE_NO_WARNINGS

/*++

OutputDebugStringの文字を取り出す。
32bit/64bitどちらでもコンパイル可能なようにしている。(ソリューションやプロジェクトの設定的にも)
・ターゲットのプロセスの｢クラス名｣がわかっているならば、ビット数の壁を越えられる。
・ターゲットのファイル名しか分かっていないならば、ビット数の壁は超えられない。
　この場合、ターゲットのビット数(32 or 64)と同じビット数でコンパイルしておく必要がある。
 なお、OutputDebugString自体の取得は、64bitと32bit混在でも問題なく取ってこれる。

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


// 以下ターゲットのクラス名か、プロセスネーム(の一部)のどちらかわかってればなんとかなる。
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

map<string, int> mapstr; // 継続的にログが出ている場合のバッファ;


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
// メイン関数
void InitDebugMonitor(HWND hEdit) {


	//--------------- セキュリティ
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = &sd;

	if (!InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION)) {
		sprintf(szDebugMonErrorMsg, "秀丸デバッガー:InitializeSecurityDescriptorすることが出来ません。エラー番号 == %d\n",
			GetLastError());
		MessageBox(NULL, szDebugMonErrorMsg, szDebugMonErrorMsg, NULL);
		exit(1);
	}

	if(!SetSecurityDescriptorDacl(&sd, TRUE, (PACL)NULL, FALSE)) {
		sprintf(szDebugMonErrorMsg, "秀丸デバッガー:SetSecurityDescriptorDaclすることが出来ません。エラー番号 == %d\n",
			GetLastError());
		MessageBox(NULL, szDebugMonErrorMsg, szDebugMonErrorMsg, NULL);
		exit(1);
	}

	// OutputDebugStringで書き込んだよーというイベントを取得するためにイベント作成。
	//--------------- ACKイベント(シグナル状態で、次のOutputDebugString()用意)
	AckEvent = CreateEvent(&sa, FALSE, FALSE, "DBWIN_BUFFER_READY");

	if (!AckEvent) {
		sprintf(szDebugMonErrorMsg, "秀丸デバッガー:同期オブジェクトを生成することが出来ません。エラー番号 == %d\n",
			GetLastError());
		MessageBox(NULL, szDebugMonErrorMsg, szDebugMonErrorMsg, NULL);
		exit(1);
	}

	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		sprintf(szDebugMonErrorMsg, "秀丸デバッガー: すで別のデバッグモニター系列のアプリケーションが実行されています。\n");
	//	MessageBox(NULL, szDebugMonErrorMsg, szDebugMonErrorMsg, NULL);
	//	exit(1);
	}

	//--------------- READYイベント(シグナル状態でOutputDebugString()出力が完了)
	ReadyEvent = CreateEvent(&sa, FALSE, FALSE, "DBWIN_DATA_READY");

	if (!ReadyEvent) {
		sprintf(szDebugMonErrorMsg, "秀丸デバッガー:同期オブジェクトを生成することが出来ません。エラー番号 == %d\n",
			GetLastError());
		MessageBox(NULL, szDebugMonErrorMsg, szDebugMonErrorMsg, NULL);
		exit(1);
	}

	// OutputDebugString()で書き込まれるバッファを共有メモリで開く
	//--------------- "DBWIN_BUFFER"という名称の共有メモリ(4096bytes)
	SharedFile = CreateFileMapping(
		(HANDLE)-1,
		&sa,
		PAGE_READWRITE,
		0,
		4096,
		"DBWIN_BUFFER");

	if (!SharedFile) {
		sprintf(szDebugMonErrorMsg, "秀丸デバッガー:ファイルマッパーオブジェクトを生成することが出来ません。エラー番号 == %d\n",
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
		sprintf(szDebugMonErrorMsg, "秀丸デバッガー:共用メモリオブジェクトを生成することが出来ません。エラー番号 == %d\n", 
			GetLastError());
		MessageBox(NULL, szDebugMonErrorMsg, szDebugMonErrorMsg, NULL);
		exit(1);
	}

	//--------------- 先頭DWORDがプロセスID、以下が格納文字列
	String = (LPSTR)SharedMem + sizeof(DWORD);
	pThisPid = (LPDWORD)SharedMem;

	LastPid = 0xffffffff;
	DidCR = TRUE;

	// 準備完了まで待つ
	SetEvent(AckEvent);
}

//  ターゲットのプロセスIDが、秀丸以下のtooltips_class32というウィンドウハンドルか
static bool IsTargetProcessIsHidemaruToolTipsWindow(const DWORD dwPID)	// プロセスID
{
	HWND hWnd = FindWindow(TARGET_CLASS_WND, NULL);

	// 無かったらストアアプリ版も
	if (!hWnd)  { hWnd = FindWindow(TARGET_CLASS_APP_WND, NULL); }

	// 両方無い
	if (!hWnd) {
		return false;
	}

	// プロセスハンドルを取得
	HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
	if (!hProcess) {
		return false;
	}

	// 実行可能ファイル名を取得
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
// スレッド
unsigned __stdcall ThreadExternalProcDebugMonitor(void *lpx) {

	for (;;) {

		ret = WaitForSingleObject(ReadyEvent, INFINITE);

		// -----------すでに存在する場合の処理
		// ウィンドウタイプの場合も考えて、アイコン化されていたら、元へと戻す。
		HWND hWndDebugMonitor = FindWindow(szClassName, NULL);
		SetWindowPos(hWndDebugMonitor, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);


		if (ret != WAIT_OBJECT_0) {

			sprintf(szDebugMonErrorMsg, "秀丸デバッガー: 待機エラー。エラー番号 == %dn",
				GetLastError());
			MessageBox(NULL, szDebugMonErrorMsg, szDebugMonErrorMsg, NULL);
			exit(1);

		} else {
			// 以下受け取った文字列をどうこうする処理

			if (LastPid != *pThisPid) {

				LastPid = *pThisPid;
				if (!DidCR) {
					// putchar('\n');
					DidCR = TRUE;
				}
			}

			// ターゲットのProcessIDのものだけ表示する。
			// 毎度毎度チェックする必要がある。

			// ターゲットのプロセスＩＤ
			bool hWndTargetProcessIsHidemaruToolTipsWindow = IsTargetProcessIsHidemaruToolTipsWindow(*pThisPid);

			// printf("%s", String); // utf8
			string sjis = String;

			if (sjis == ("xxxxxxxxxx\n")) {
				// 受信を停止していない。直前に秀丸を起動したと言ってない。
				if (iPrevIsHidemaruStart == FALSE) {
					WriteRichEdit(hEdit, "秀丸の再起動を認識。\n\n");
					iPrevIsHidemaruStart = TRUE;
					hWndTargetProcessIsHidemaruToolTipsWindow = false;
					AllRecievedLog.push_back("秀丸の再起動を認識。\n\n");
				}
				else {
					hWndTargetProcessIsHidemaruToolTipsWindow = false;
				}
			}
			else {
				iPrevIsHidemaruStart = FALSE;
			}

			// 毎回ストック
			sjisLineStock += sjis;

			// 改行が存在しない
			if (sjis.find("\n") == string::npos) {
				// 行末を含んでいない
				isSjisLineEnd = 0;
			}
			// 行末を含む
			else {
				// ストックした分を渡す
				sjis = sjisLineStock;
				// 渡したのでストックはクリア
				sjisLineStock = "";
				// 行末モード
				isSjisLineEnd = 1;
			}

			string from = "showvars";
			string::size_type pos = sjis.find(from);
			if (pos != string::npos) {
				sjis.replace(pos, from.size(), "変数一覧");
			}

			//これはファイルの保存や読み込みでなぜか出る
			if (sjis.find("shell\\comdlg32\\fileopensave.cpp(") != string::npos) {
				hWndTargetProcessIsHidemaruToolTipsWindow = false;
			}

			if (isSjisLineEnd) {
				// utf8系という判定が出たら、sjisへ
				enum NMH_CHAR_CODE n = nmh_code_detect(sjis.c_str(), strlen(sjis.c_str()));
				const char *szname = NMH_CHAR_CODE_NAME_LIST[n];
				if (strstr(szname, "UTF8")) {
					sjis = utf8_to_cp932(sjis);
				}
			}

			// ターゲットプロセスが立ち上がっている時には、ターゲットプロセス以外のものは出力しないようにする(邪魔になるから)
			if (isSjisLineEnd && isDoingRecieveFlag && hWndTargetProcessIsHidemaruToolTipsWindow) {
				
				if (DidCR) {
					printf("\n------------------------------------------------ プロセスID:%3u:からの受信\n", LastPid);
				}

				int modePrint = 1;

				// 800ミリ秒インタラプトがあれば、継続はしていないと考える。
				if ( int(::timeGetTime() - preTimeGetTime) > 800 ) {
					// 継続を解除し、
					lstIntrGetTime = ::timeGetTime();
					// 登録していたマップをクリア
					mapstr.clear();
				}

				// ８秒間以上続いているならば、その間にあった文字は表示しない。
				if ( int(::timeGetTime() - lstIntrGetTime) > 8000 ) {
					// すでに登録してあったら出力しない。
					if ( mapstr[sjis] ) {
						modePrint = 0;
					}
				}

				// ３秒間で続けている。
				if ( int(::timeGetTime() - lstIntrGetTime) > 3000 ) {

					// ハッシュのカウンタ加算。
					mapstr[sjis] += 1;
				}

				// １つ前にメッセージを吐き出した時間として現在の時間を入れる
				preTimeGetTime = ::timeGetTime();

				// 前回省略とプリントしてから２秒経過
				// 省略パターンを出すため、modePrint2とする。
				if ( modePrint == 0 && int(::timeGetTime() - minPrintGetTime) > 2000 ) {
					minPrintGetTime = ::timeGetTime();
					modePrint = 2;
				}

				// 出力すべきモード
				if ( modePrint ) {

					// グローバルにコピー
					strncpy( szBufSendMessage, sjis.c_str(), sizeof(szBufSendMessage) );
					szBufSendMessage[sizeof(szBufSendMessage)-1] = NULL;

					// すでにサイズがデカすぎる場合、小さくする。
					// OVER_ALL_RECIEVED_LOG_SIZE行を超える
					if ( AllRecievedLog.size() > OVER_ALL_RECIEVED_LOG_SIZE ) {
						// 最後100行分コピー
						vector<string> tmp;
						for ( int i=int(AllRecievedLog.size()-JUST_ALL_RECIEVED_LOG_SIZE); i < int(AllRecievedLog.size()); i++ ) {
							tmp.push_back( AllRecievedLog[i] );
						}
						// 100行分にデータ書き換え
						AllRecievedLog = tmp;
						// 100行分で表示しなおし
						ReWriteAllRichEdit(hEdit);
					}

					if ( strstr(szBufSendMessage, "HIDEMARUERR: " ) ) {
						iHidemaruErrorCnt++;
					}

					// 通常のプリント
					if ( modePrint == 1 ) {
						// printf("%s", sjis.c_str() );
						AllRecievedLog.push_back( szBufSendMessage );

						WriteRichEdit(hEdit, szBufSendMessage );

						// 省略モード
					} else if ( modePrint == 2) {
						// printf("秀丸:～同じログの繰り返しなため省略中～\n");
						AllRecievedLog.push_back(  "～同じログの繰り返しなため省略中～\n" );
						WriteRichEdit(hEdit, "～同じログの繰り返しなため省略中～\n");

						// 要素を出力する
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
						AllRecievedLog.push_back(  "\n秀丸: エラーが多くなっています。受信を停止します。\n" );
						WriteRichEdit(hEdit, "\n秀丸: エラーが多くなっています。受信を停止します。\n");

						// 受信を停止
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

