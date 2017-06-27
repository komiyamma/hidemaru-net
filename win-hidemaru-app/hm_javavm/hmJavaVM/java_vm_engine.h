#pragma once
#include <jni.h>

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

	static HMODULE hJavaVMDll;

	static JavaVM *jvm;
	static JNIEnv* env;

public:
	static void CreateVM();
	static void DestroyVM();

public:
	//-------------------------------------------------------------------------
	// Java Virtual Machineは読み込み済みでかつ有効かどうか。
	static bool IsValid();

	// 該当クラスの該当staticメソッドを実行してみよう。
	static bool GetStaticActionMethod(wstring class_name, wstring method_name);

private:
	static wstring GetErrorMessage();
};


