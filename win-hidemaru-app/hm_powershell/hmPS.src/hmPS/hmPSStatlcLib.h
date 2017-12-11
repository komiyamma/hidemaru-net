/* 
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */


#pragma once

using namespace System;

// dynamic型を利用しないもの。dynamicを呼び出すだけ。こうすることで、C++とリンクすることができる(メソッドの個数を合わせることが出来る)
public ref class IPSStaticLib
{
public:
	static void OutputDebugStream(String^ error);

	static IntPtr CreateScope();

	static IntPtr BindDllHandle(IntPtr dll);

	static void SetCodePage(IntPtr cp);

	static IntPtr SetTmpVar(Object^ value);

	static Object^ PopTmpVar();

	//-----------------------------------------------------------------
	static IntPtr GetNumVar(String^ mng_var_name);

	static IntPtr SetNumVar(String^ mng_var_name, IntPtr value);

	static String^ GetStrVar(String^ mng_var_name);

	static IntPtr SetStrVar(String^ mng_var_name, String^ value);
	//-----------------------------------------------------------------


	static IntPtr DoString(String^ expression);

	static IntPtr DoFile(String^ filename);

	static IntPtr DestroyScope();

};