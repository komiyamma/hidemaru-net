/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the MIT License
 */

#pragma once

using namespace System;
using namespace System::Collections::Generic;

// dynamic�^�𗘗p���Ȃ����́Bdynamic���Ăяo�������B�������邱�ƂŁAC++�ƃ����N���邱�Ƃ��ł���(���\�b�h�̌������킹�邱�Ƃ��o����)
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