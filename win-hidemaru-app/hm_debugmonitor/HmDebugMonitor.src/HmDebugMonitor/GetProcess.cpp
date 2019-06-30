/*
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the MIT License
 */

#include <windows.h>

#include <stdio.h>
#include <psapi.h>  
#include <tlhelp32.h>

#pragma comment(lib,"psapi.lib")



// szNameがフルパスに含まれるプロセスＩＤを取得。最初に見つかったやつ。
// だが確実性にやや欠けるので、可能な限りクラス名(TARGET_CLASS_WND)で対処した方がよい。
int getTargetProcessID ( char *szNamePattern )
{
	DWORD ProcessID[ 1024 ];
	DWORD dwSize;
	DWORD dwMax;
	DWORD dwNow;
	HANDLE hProcess;

	// プロセス識別子の取得
	EnumProcesses( ProcessID, sizeof(ProcessID), &dwSize );
	dwMax = (dwSize / sizeof(DWORD));
	// プロセス識別子からプロセス名を列挙
	for ( dwNow = 0 ; dwNow < dwMax ; dwNow++ ){
		TCHAR   szFile[ 1024 ] = { 0 };
		HMODULE Module[ 1024 ] = { 0 };

		// プロセスのフルパス名を取得
		if ( (hProcess = OpenProcess((PROCESS_QUERY_INFORMATION | PROCESS_VM_READ),FALSE,ProcessID[dwNow])) != NULL ){
			if ( EnumProcessModules(hProcess,Module,sizeof(Module),&dwSize) ){
				GetModuleFileNameEx( hProcess, Module[0], szFile, sizeof(szFile) );
			}
			CloseHandle( hProcess );
		}

		if ( strstr(szFile, szNamePattern) ) {
			printf( "%3d: %08d [%s]\n", (dwNow + 1), ProcessID[dwNow], szFile );
			return ProcessID[dwNow];
		}
	}
	return 0;
}





// 指定のファイル名のプロセス名が何個実行されいるかを得る
int getTargetProcessCount(char* szFileName) {
	int iMainReturnFlag = 0;

	HANDLE hthSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hthSnapshot == INVALID_HANDLE_VALUE) {
		printf("Error! 0x%.8x (%d)\n", GetLastError(), __LINE__);
		return -1;
	}

	PROCESSENTRY32 pe = { sizeof(PROCESSENTRY32) };
	BOOL bProcess = Process32First(hthSnapshot, &pe);

	for (; bProcess; bProcess = Process32Next(hthSnapshot, &pe)) {
		/*
		printf("PROCESSENTRY32.dwSize              = %d\n", pe.dwSize);
		printf("PROCESSENTRY32.cntUsage            = %d\n", pe.cntUsage);
		printf("PROCESSENTRY32.th32ProcessID       = %d\n", pe.th32ProcessID);
		printf("PROCESSENTRY32.th32DefaultHeapID   = %p\n", pe.th32DefaultHeapID);
		printf("PROCESSENTRY32.th32ModuleID        = %d\n", pe.th32ModuleID); 
		printf("PROCESSENTRY32.cntThreads          = %d\n", pe.cntThreads ); 
		printf("PROCESSENTRY32.th32ParentProcessID = %d\n", pe.th32ParentProcessID ); 
		printf("PROCESSENTRY32.pcPriClassBase      = %d\n", pe.pcPriClassBase ); 
		printf("PROCESSENTRY32.dwFlags             = 0x%.8x\n", pe.dwFlags ); 
		printf("PROCESSENTRY32.szExeFile           = %S\n\n", pe.szExeFile );
		*/
		if (strcmp(pe.szExeFile, szFileName) == 0) {
			iMainReturnFlag++;
		}
	}

	CloseHandle(hthSnapshot);

	return iMainReturnFlag;
}
