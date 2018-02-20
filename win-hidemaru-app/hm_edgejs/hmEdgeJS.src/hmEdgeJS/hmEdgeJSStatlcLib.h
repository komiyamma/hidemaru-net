/* 
 * Copyright (c) 2018 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */


#pragma once

using namespace System;

// dynamic型を利用しないもの。dynamicを呼び出すだけ。こうすることで、C++とリンクすることができる(メソッドの個数を合わせることが出来る)
public ref class IEdgeJSStaticLib
{
public:
	static void OutputDebugStream(String^ error);

	static IntPtr CreateScope();

	static IntPtr BindDllHandle(IntPtr dll);

	static IntPtr SetTmpVar(Object^ value);

	static Object^ PopTmpVar();

	static IntPtr DoFile(String^ filename);

	static IntPtr DestroyScope();

};