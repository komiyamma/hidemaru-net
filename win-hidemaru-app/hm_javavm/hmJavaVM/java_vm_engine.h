/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#pragma once
#include "jni.h"

#include <tchar.h>
#include <windows.h>
#include <regex>
#include <string>


using namespace std;


class CJavaVMEngine {

private:
	//-------------------------------------------------------------------------
	// jvm.dll��Java Virtual Machine�BJava����ɓ��肳��Ȃ��AJava Virtual Machine���̂��́B
	static const int jni_version = JNI_VERSION_1_8;

	// jvm.dll�̃n���h��
	static HMODULE hJavaVMDll;
private:
	// hJavaVMDLL�̃t���p�X
	static wstring GetHavaVMDllPath();
	static wstring GetHavaVMDllPathFromSettingsFile();
private:
	// "-Djava.class.path=C:\\usr\\hidemaru;C:\\usr\\hidemaru\\hmJavaVM.jar"; // �Ƃ����`�ɂ���B
	static char szJavaClassPathBuffer[512];


	static JavaVM *jvm;
	static JNIEnv* env;

public:
	static void CreateVM();
	static void DestroyVM();

public:
	//-------------------------------------------------------------------------
	// Java Virtual Machine�͓ǂݍ��ݍς݂ł��L�����ǂ����B
	static bool IsValid();
public:

	static bool HmCalled;

	// �Y���N���X�̊Y��static���\�b�h�����s���Ă݂悤�B
	static bool CallStaticEntryMethod(wstring class_name, wstring method_name);
	static jlong CallStaticEntryMethodOfLong(wstring class_name, wstring method_name, string method_args_typedef_string = "([Ljava / lang / String;)V", string method_args_declare_string = "String[]");
	static wstring CallStaticEntryMethodOfString(wstring class_name, wstring method_name, string method_args_typedef_string = "([Ljava / lang / String;)V", string method_args_declare_string = "String[]");

	static void GC();

private:
	static wstring GetErrorMessage();
};


