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
	// jvm.dllのJava Virtual Machine。Java言語に特定されない、Java Virtual Machineそのもの。
	static const int jni_version = JNI_VERSION_1_8;

	// jvm.dllのハンドル
	static HMODULE hJavaVMDll;
private:
	// hJavaVMDLLのフルパス
	static wstring GetHavaVMDllPath();
	static wstring GetHavaVMDllPathFromSettingsFile();
private:
	// "-Djava.class.path=C:\\usr\\hidemaru;C:\\usr\\hidemaru\\hmJavaVM.jar"; // という形にする。
	static char szJavaClassPathBuffer[512];


	static JavaVM *jvm;
	static JNIEnv* env;

public:
	static void CreateVM();
	static void DestroyVM();

public:
	//-------------------------------------------------------------------------
	// Java Virtual Machineは読み込み済みでかつ有効かどうか。
	static bool IsValid();
public:

	static bool HmCalled;

	// 該当クラスの該当staticメソッドを実行してみよう。
	static bool CallStaticEntryMethod(wstring class_name, wstring method_name);

	// ---------- LONG を返す --------------
	// 引数なし、かつデフォルトの数値返し （過去の互換のために static void funcname(String[] args) 型が素で呼び出せる
	static jlong CallStaticEntryMethodOfLong(wstring class_name, wstring method_name, string method_args_typedef_string = "([Ljava / lang / String;)V", string method_args_declare_string = "String[]");
	// 数値の引数１つ
	static jlong CallStaticEntryMethodOfLong(wstring class_name, wstring method_name, jlong a1, string method_args_typedef_string, string method_args_declare_string);
	// 文字列の引数１つ
	static jlong CallStaticEntryMethodOfLong(wstring class_name, wstring method_name, wstring s1, string method_args_typedef_string, string method_args_declare_string);
	// 数値の引数２つ
	static jlong CallStaticEntryMethodOfLong(wstring class_name, wstring method_name, jlong a1, jlong a2, string method_args_typedef_string, string method_args_declare_string);
	// 文字列１つと、数値１つ
	static jlong CallStaticEntryMethodOfLong(wstring class_name, wstring method_name, wstring s1, jlong a2, string method_args_typedef_string, string method_args_declare_string);
	// 数値１つと文字列１つ
	static jlong CallStaticEntryMethodOfLong(wstring class_name, wstring method_name, jlong a1, wstring s2, string method_args_typedef_string, string method_args_declare_string);

	// ---------- STRING を返す --------------
	// 引数なし
	static wstring CallStaticEntryMethodOfString(wstring class_name, wstring method_name, string method_args_typedef_string, string method_args_declare_string);
	// 数値の引数１つ
	static wstring CallStaticEntryMethodOfString(wstring class_name, wstring method_name, jlong a1, string method_args_typedef_string, string method_args_declare_string);
	// 文字列の引数１つ
	static wstring CallStaticEntryMethodOfString(wstring class_name, wstring method_name, wstring s1, string method_args_typedef_string, string method_args_declare_string);
	// 数値の引数２つ
	static wstring CallStaticEntryMethodOfString(wstring class_name, wstring method_name, jlong a1, jlong a2, string method_args_typedef_string, string method_args_declare_string);
	// 文字列１つと、数値１つ
	static wstring CallStaticEntryMethodOfString(wstring class_name, wstring method_name, wstring a1, jlong a2, string method_args_typedef_string, string method_args_declare_string);
	// 数値１つと、文字列１つ
	static wstring CallStaticEntryMethodOfString(wstring class_name, wstring method_name, jlong a1, wstring s2, string method_args_typedef_string, string method_args_declare_string);



	static void GC();

private:
	static wstring GetErrorMessage();
};


