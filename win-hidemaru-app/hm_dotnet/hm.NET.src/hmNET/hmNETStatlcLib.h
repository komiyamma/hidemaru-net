/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the MIT License
 */

#pragma once

using namespace System;
using namespace System::Collections::Generic;

// dynamic型を利用しないもの。dynamicを呼び出すだけ。こうすることで、C++とリンクすることができる(メソッドの個数を合わせることが出来る)
private ref class INETStaticLib
{
public:
	static void OutputDebugStream(String^ error);

	static void TraceMethodInfo(String^ assm_path, String^ class_name, String^ method_name);

	static void TraceExceptionInfo(Exception^ e);

	static IntPtr AttachScope();

	static IntPtr BindDllHandle(IntPtr dll);

	static IntPtr SetTmpVar(Object^ value);

	static Object^ PopTmpVar();

	static IntPtr CallMethod(String^ expression);

	static Object^ SubCallMethod(String^ assm_path, String^ class_name, String^ method_name, List<Object^>^ args, String^ func_mode);

	static IntPtr DetachScope(IntPtr status);

};