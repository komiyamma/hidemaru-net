#pragma once

using namespace System;

// dynamic型を利用しないもの。dynamicを呼び出すだけ。こうすることで、C++とリンクすることができる(メソッドの個数を合わせることが出来る)
private ref class INETStaticLib
{
public:
	static void OutputDebugStream(String^ error);

	static IntPtr AttachScope();

	static IntPtr BindDllHandle(IntPtr dll);

	static IntPtr SetTmpVar(Object^ value);

	static Object^ PopTmpVar();

	static IntPtr CallMethod(String^ expression);

	static IntPtr DetachScope();

};