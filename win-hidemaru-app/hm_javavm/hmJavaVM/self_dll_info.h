/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#pragma once

#include <windows.h>
#include <tchar.h>
#include <string>

using namespace std;

//-------------------------------------------------------------------------
// dll自身のハンドルやフスパスの情報の保持
//-------------------------------------------------------------------------
struct CSelfDllInfo {

	//-------------------------------------------------------------------------
	// 自分自身(hmPerl.dll)のモジュールインスタンスハンドル
	static HMODULE hModule;

	//-------------------------------------------------------------------------
	// 自分自身(hmPerl.dll)のフルパス
	static TCHAR szSelfModuleFullPath[MAX_PATH];

	static TCHAR szSelfModuleDirPath[MAX_PATH];


	//-------------------------------------------------------------------------
	// このdllが秀丸マクロからどのような形でloaddllされたのかの情報。
	// この情報があれば、dll内部からマクロを発行することが出来る。
	// -1   :loaddll文で束縛だれた
	// 0    :読めていない。(読めてなかったらdll実行されてないので、これはあり得ない)
	// 1以上:その数値で秀丸マクロ上で束縛されている。
	//-------------------------------------------------------------------------
private:
	static int iSelfBindedType;
public:
	static void InitializeHandle(HMODULE handle);

	static int GetBindDllType();
	static BOOL SetBindDllHandle();
	static wstring GetInvocantString();
	static wstring GetSelfModuleFullPath();
	static wstring GetSelfModuleDir();
};


