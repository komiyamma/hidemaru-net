/*
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#pragma once

#include <windows.h>
#include <string>

using namespace std;

/*--------------------------------------------------------------------------------------------------------------------------
int WINAPI Hidemaru_GetDllFuncCalledType(int n);
① nに - 1を指定すると、loaddllのされ方を返します。1以上でloaddll関数の返り値と同じです。
　 - 1の場合はloaddll文で読み込まれたDLLです。

  ② nに0を指定すると、dllfunc / dllfuncw / dllfuncstr / dllfuncstrwのどれで呼ばれたかによって、returnで返すべき値を示します。
  　 以下の値のいずれかが返ります。
	--------------------------------------------------------------------------------------------------------------------------*/
#define DLLFUNCRETURN_NOTCALL   0       //呼ばれていない
#define DLLFUNCRETURN_INT       0x0001  //intを返すべき(dllfuncまたはdllfuncw)
#define DLLFUNCRETURN_CHAR_PTR  0x0002  //char*を返すべき(dllfuncstr)
#define DLLFUNCRETURN_WCHAR_PTR 0x0003  //WCHAR*を返すべき(dllfuncstrw)
#define DLLFUNCRETURN_DOUBLE    0x0004  //doubleを返すべき(dllfuncまたはdllfuncwで浮動小数点数が有効)

/*--------------------------------------------------------------------------------------------------------------------------
③ nに1から数えた順番の値を指定すると、1から数えたパラメータの種類が数値 / 文字列 / Unicode文字列 / 浮動小数点数かを返します。
　 パラメータの最大は64個です。
  以下の値のいずれかが返ります。
  --------------------------------------------------------------------------------------------------------------------------*/
#define DLLFUNCPARAM_NOPARAM    0       //以降のパラメータなし
#define DLLFUNCPARAM_INT        0x0100  //intのパラメータ
#define DLLFUNCPARAM_CHAR_PTR   0x0200  //char*のパラメータ
#define DLLFUNCPARAM_WCHAR_PTR  0x0300  //WCHAR*のパラメータ
#define DLLFUNCPARAM_DOUBLE     0x0400  //doubleのパラメータ

/*--------------------------------------------------------------------------------------------------------------------------
dllfunc等を呼んでいる間、最後にdllfunc等を呼んだ内容のみ取得できます。
例えばdllfunc関数内でHidemaru_EvalMacroでさらにdllfuncを呼び出す場合、取得できるものは一番新しく呼んだもののみです。
--------------------------------------------------------------------------------------------------------------------------*/

// hidemaru.exeから出ている関数を取得。原則C#層で取得するが、この関数はC++インターフェイス用なので、例外的にここで取得
using PFNGetDllFuncCalledType = int(WINAPI *)(int n);

extern PFNGetDllFuncCalledType Hidemaru_GetDllFuncCalledType;



class CHidemaruExeExport {
public:
	CHidemaruExeExport();
private:
	static HMODULE hHideExeHandle;
	static TCHAR szHidemaruFullPath[MAX_PATH];

private:
	//========================================================================
	// 秀丸本体のバージョン。
	//-------------------------------------------------------------------------
	// 8.66β3なら、「866.03」みたいな感じになる。
	//========================================================================
	static double hm_version;
	static double QueryFileVersion(TCHAR* path);

protected:
	//========================================================================
	/// 秀丸本体からExportされているいくつかの関数群
	//========================================================================

	//-------------------------------------------------------------------------
	// 現在編集中の全てのテキストを得る
	using PFNGetTotalTextUnicode = HGLOBAL(WINAPI *)(void);
	static PFNGetTotalTextUnicode Hidemaru_GetTotalTextUnicode;

	using PFNGetCurrentWindowHandle = HWND(WINAPI *)(void);
	static PFNGetCurrentWindowHandle Hidemaru_GetCurrentWindowHandle;

public:


	//========================================================================
	/// ラップ関数郡
	//========================================================================

	//-------------------------------------------------------------------------
	// Hidemaru_GetTotalTextUnicode関数のラップしたもの
	//-------------------------------------------------------------------------
	static wstring GetTotalText();

private:
	static HWND hCurWndHidemaru;
public:
	static HWND GetCurWndHidemaru();
};



