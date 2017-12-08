/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the MIT License
 */

#pragma once

using namespace System;

// dynamic�^�𗘗p���Ȃ����́Bdynamic���Ăяo�������B�������邱�ƂŁAC++�ƃ����N���邱�Ƃ��ł���(���\�b�h�̌������킹�邱�Ƃ��o����)
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