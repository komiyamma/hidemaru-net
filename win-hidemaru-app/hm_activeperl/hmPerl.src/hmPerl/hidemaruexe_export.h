/*
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#pragma once

#include <windows.h>
#include <string>

using namespace std;

class CHidemaruExeExport {
private:
	static HMODULE hHideExeHandle;
	static TCHAR szHidemaruFullPath[MAX_PATH];

public:
	//========================================================================
	// 秀丸本体のバージョン。
	//-------------------------------------------------------------------------
	// 8.66β3なら、「866.03」みたいな感じになる。
	//========================================================================
	static double hm_version;
private:
	static double QueryFileVersion(TCHAR* path);

public:
	//========================================================================
	/// 秀丸本体からExportされているいくつかの関数群
	//========================================================================

	//-------------------------------------------------------------------------
	// 現在編集中の全てのテキストを得る
	using PFNGetTotalTextUnicode = HGLOBAL(WINAPI *)(void);
	static PFNGetTotalTextUnicode Hidemaru_GetTotalTextUnicode;

	//-------------------------------------------------------------------------
	// 現在編集中の選択テキストを得る(単純選択のみ)
	using PFNGetSelectedTextUnicode = HGLOBAL(WINAPI *)(void);
	static PFNGetSelectedTextUnicode Hidemaru_GetSelectedTextUnicode;

	//-------------------------------------------------------------------------
	// 現在編集中のテキストのカーソルがある行のテキストを得る
	using PFNGetLineTextUnicode = HGLOBAL(WINAPI *)(int nLineNo);
	static PFNGetLineTextUnicode Hidemaru_GetLineTextUnicode;

	//-------------------------------------------------------------------------
	// 現在編集中のテキストのカーソルの位置を取得する。マクロのcolumnとlineno相当
	using PFNGetCursorPosUnicode = BOOL(WINAPI *)(int* pnLineNo, int* pnColumn);
	static PFNGetCursorPosUnicode Hidemaru_GetCursorPosUnicode;

	using PFNGetCursorPosUnicodeFromMousePos = BOOL(WINAPI *)(POINT* ppt, int* pnLineNo, int* pnColumn);
	static PFNGetCursorPosUnicodeFromMousePos Hidemaru_GetCursorPosUnicodeFromMousePos;

	//-------------------------------------------------------------------------
	// dllの中から秀丸マクロを実行する
	using PFNEvalMacro = BOOL(WINAPI *)(const wchar_t* pwsz);
	static PFNEvalMacro Hidemaru_EvalMacro;

	//-------------------------------------------------------------------------
	// dllの呼ばれかた、関数の呼ばれ方(返り値の要求型や引数の要求型を判定できる関数)。強力
	//-------------------------------------------------------------------------
	// dllfunc等を呼んでいる間、最後にdllfunc等を呼んだ内容のみ取得できます。
	// 例えばdllfunc関数内でHidemaru_EvalMacroでさらにdllfuncを呼び出す場合、取得できるものは一番新しく呼んだもののみです。
	//-------------------------------------------------------------------------
	using PFNGetDllFuncCalledType = int(WINAPI *)(int n);
	static PFNGetDllFuncCalledType Hidemaru_GetDllFuncCalledType;

	//-------------------------------------------------------------------------
	// nに - 1を指定すると、loaddllのされ方を返します。1以上でloaddll関数の返り値と同じです。 - 1の場合はloaddll文で読み込まれたDLLです。

	//-------------------------------------------------------------------------
	// nに0を指定すると、dllfunc / dllfuncw / dllfuncstr / dllfuncstrwのどれで呼ばれたかによって、returnで返すべき値を示します。
	// 以下の値のいずれかが返ります。
	// dllfunc等が呼ばれているとき、どのような呼ばれ方をしているかを取得します。
	enum class DLLFUNCRETURN {
		NOTCALL   = 0,       //呼ばれていない
		INT       = 0x0001,  //intを返すべき(dllfuncまたはdllfuncw)
		CHAR_PTR  = 0x0002,  //char*を返すべき(dllfuncstr)
		WCHAR_PTR = 0x0003,  //WCHAR*を返すべき(dllfuncstrw)
		DOUBLE    = 0x0004  //doubleを返すべき(dllfuncまたはdllfuncwで浮動小数点数が有効)
	};

	//-------------------------------------------------------------------------
	//nに1から数えた順番の値を指定すると、1から数えたパラメータの種類が数値 / 文字列 / Unicode文字列 / 浮動小数点数かを返します。
	// パラメータの最大は64個です。
	// 以下の値のいずれかが返ります。
	enum class DLLFUNCPARAM {
		NOPARAM   = 0,       //以降のパラメータなし
		INT       = 0x0100,  //intのパラメータ
		CHAR_PTR  = 0x0200,  //char*のパラメータ
		WCHAR_PTR = 0x0300,  //WCHAR*のパラメータ
		DOUBLE    = 0x0400   //doubleのパラメータ
	};

public:

	//========================================================================
	/// 初期化
	//========================================================================
	static BOOL init();


	//========================================================================
	/// ラップ関数郡
	//========================================================================

	//-------------------------------------------------------------------------
	// Hidemaru_GetTotalTextUnicode関数のラップしたもの
	//-------------------------------------------------------------------------
	static wstring GetTotalText();

	//-------------------------------------------------------------------------
	// Hidemaru_GetSelectedTextUnicode関数のラップしたもの
	//-------------------------------------------------------------------------
	static wstring GetSelectedText();

	//-------------------------------------------------------------------------
	// Hidemaru_GetLineTextUnicode関数のラップしたもの
	//-------------------------------------------------------------------------
	static wstring GetLineText(int lineno = 0);

	//-------------------------------------------------------------------------
	// Hidemaru_GetCursorPosUnicode関数のラップしたもの
	//-------------------------------------------------------------------------
	// 座標型。Point型では、System.Drawingを読み込まないとダメなので負荷がある。また、x, yは秀丸に別値として存在するので、
	// あくまでも、マクロのcolumnとlinenoと一致しているという主張。
	// columnやlinenoはエディタ的な座標である。
	struct HmCursurPos
	{
	public:
		int lineno;
		int column;
		HmCursurPos(int _lineno, int _column)
		{
			this->lineno = _lineno;
			this->column = _column;
		}
	};
	static HmCursurPos GetCursorPos();


	//-------------------------------------------------------------------------
	// Hidemaru_GetCursorPosUnicodeFromMousePos関数のラップしたもの
	// (秀丸8.73以上)
	//-------------------------------------------------------------------------
	struct HmMousePos
	{
	public:
		int x;
		int y;
		int lineno;
		int column;
		HmMousePos(int _x, int _y, int _lineno, int _column)
		{
			this->x = _x;
			this->y = _y;
			this->lineno = _lineno;
			this->column = _column;
		}
	};
	static HmMousePos GetCursorPosFromMousePos();

	//-------------------------------------------------------------------------
	// Hidemaru_EvalMacro関数のラップしたもの
	//-------------------------------------------------------------------------
	static BOOL EvalMacro(wstring);


};


