/* 
 * Copyright (c) 2018 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */


#pragma once

using namespace System;

// dynamic�^�𗘗p���Ȃ����́Bdynamic���Ăяo�������B�������邱�ƂŁAC++�ƃ����N���邱�Ƃ��ł���(���\�b�h�̌������킹�邱�Ƃ��o����)
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