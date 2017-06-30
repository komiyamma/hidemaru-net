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

private:
	static wstring GetErrorMessage();
};


