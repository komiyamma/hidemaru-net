#pragma once
#include <jni.h>

#include <tchar.h>
#include <windows.h>
#include <regex>
#include <string>


using namespace std;


class CJavaVMEngine {

private:

	static HMODULE hJavaVMDll;
private:
	//-------------------------------------------------------------------------
	// jvm.dllのJava Virtual Machine。Java言語に特定されない、Java Virtual Machineそのもの。
	static const int jni_version = JNI_VERSION_1_8;

	static JavaVM *jvm;
	static JNIEnv* env;

private:
	//========================================================================
	/// jvm.dllからExportされているいくつかの関数群の型保持
	//========================================================================

public:



public:
	static void CreateVM();
	static void DestroyVM();

private:

public:
	//-------------------------------------------------------------------------
	// エンジンは有効(読み込み済)かどうか。
	static bool IsValid();

	static bool GetStaticActionMethod(wstring class_name, wstring method_name);
	/*
	// 対象の文字列はパールEvalエラーぽい。
	BOOL IsReturnBufferSeemsEvalError(char *utf8_buffer);

	struct TEvalErrorInfo {
		bool HasError;		// エラーがあったかどうか。
		int lineno;			// エラー行
		int column;			// エラー位置
		wstring errorlinecode;	// エラーが出た行の文字列
	};

	// エラーの位置と情報を分析
	TEvalErrorInfo GetEvalErrorInfo(wstring strAllSrcCode, char *utf8_buffer);
	*/
};


