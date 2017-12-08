/* 
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */


#pragma once

using namespace System;

// dynamic型を利用しないもの。dynamicを呼び出すだけ。こうすることで、C++とリンクすることができる(メソッドの個数を合わせることが出来る)
public ref class IRbStaticLib
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

	//-----------------------------------------------------------------
	static IntPtr GetNumItemOfList(String^ mng_arr_name, IntPtr index);

	static IntPtr SetNumItemOfList(String^ mng_arr_name, IntPtr index, IntPtr value);

	static String^ GetStrItemOfList(String^ mng_arr_name, IntPtr index);

	static IntPtr SetStrItemOfList(String^ mng_arr_name, IntPtr index, String^ value);
	//-----------------------------------------------------------------

	//-----------------------------------------------------------------
	static IntPtr GetNumItemOfDict(String^ mng_arr_name, String^ key);

	static IntPtr SetNumItemOfDict(String^ mng_arr_name, String^ key, IntPtr value);

	static String^ GetStrItemOfDict(String^ mng_arr_name, String^ key);

	static IntPtr SetStrItemOfDict(String^ mng_arr_name, String^ key, String^ value);
	//-----------------------------------------------------------------


	static IntPtr DoString(String^ expression);

	static IntPtr DoFile(String^ filename);

	static IntPtr DestroyScope();

};