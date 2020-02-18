/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#include <shlwapi.h>
#include <vector>

#include "java_vm_engine.h"
#include "self_dll_info.h"
#include "convert_string.h"
#include "output_debugstream.h"



using namespace std;

#pragma comment(lib, "shlwapi.lib")

// staticの初期化
HMODULE CJavaVMEngine::hJavaVMDll = NULL;

char CJavaVMEngine::szJavaClassPathBuffer[512] = "-Djava.class.path=";

JavaVM *CJavaVMEngine::jvm = NULL;
JNIEnv* CJavaVMEngine::env = NULL;

bool CJavaVMEngine::HmCalled = false;


void CJavaVMEngine::CreateVM() {
	if (IsValid()) {
		return;
	}

	// 読み取った値をパスとして動的にjvm.dllライブラリーをロード
	wstring jvmFullPath = GetHavaVMDllPath().c_str();
	hJavaVMDll = LoadLibrary(jvmFullPath.c_str());
	if (!hJavaVMDll) {
		MessageBox(NULL, L"jvm.dllが見つかりません。", L"jvm.dllが見つかりません。", NULL);
		return;
	}

	using TJNI_CreateJavaVM = decltype(JNI_CreateJavaVM);
	auto pCreateJavaVM = (TJNI_CreateJavaVM *)GetProcAddress(hJavaVMDll, "JNI_CreateJavaVM");
	if (!pCreateJavaVM) {
		MessageBox(NULL, L"jvm.dllにJNI_CreateJavaVM関数が見つかりません。", L"jvm.dllにJNI_CreateJavaVM関数が見つかりません。", NULL);
		return;
	}

	// JNI_CreateJavaVMを呼び出し
	JavaVMOption options[2];

//	options[0].optionString = "-Djava.class.path=C:\\usr\\hidemaru;C:\\usr\\hidemaru\\hmJavaVM.jar"; // という形にする。
	wstring dir = CSelfDllInfo::GetSelfModuleDir();
	string cp932_dir = utf16_to_cp932(dir);
//	strcat_s(szJavaClassPathBuffer, cp932_dir.c_str());
//	strcat_s(szJavaClassPathBuffer, ";");
	strcat_s(szJavaClassPathBuffer, cp932_dir.c_str());
	strcat_s(szJavaClassPathBuffer, "\\hmJavaVM.jar");
	OutputDebugStream(cp932_to_utf16(szJavaClassPathBuffer));
	options[0].optionString = szJavaClassPathBuffer;
//	options[1].optionString = "Xmx16m-Xmx128m";

	JavaVMInitArgs vm_args;
	vm_args.version = jni_version;
	vm_args.options = options;
	vm_args.nOptions = 1;

	OutputDebugStream(L"Java VM起動直前");
	env = NULL;
	jvm = NULL;
	try {
		/*
		#define JNI_OK        0      // success
		#define JNI_ERR       (-1)   // unknown error
		#define JNI_EDETACHED (-2)   // thread detached from the VM
		#define JNI_EVERSION  (-3)   // JNI version error
		#define JNI_ENOMEM    (-4)   // not enough memory
		#define JNI_EEXIST    (-5)   // VM already created
		#define JNI_EINVAL    (-6)   // invalid arguments
		*/
		int isValid = pCreateJavaVM(&jvm, (void**)&env, &vm_args);
		if (isValid < 0) {
			wstring msg = L"JNI_CreateJavaVM Error: " + std::to_wstring(isValid);
			MessageBox(NULL, msg.c_str(), msg.c_str(), NULL);
			OutputDebugStream(L"Java VM起動に失敗");
			return;
		}
	}
	catch (exception e) {
		MessageBoxA(NULL, e.what(), e.what(), NULL);
	}

	OutputDebugStream(jvmFullPath);
	OutputDebugStream(L"Java VM起動に成功");
}

void CJavaVMEngine::GC() {

	if (jvm) {
		// Helloクラスのロード
		jclass clazz = env->FindClass("java/lang/System");
		if (clazz) {
			jmethodID mid = env->GetStaticMethodID(clazz, "gc", "()V");
			if (mid) {
				// mainメソッド実行
				env->CallStaticVoidMethod(clazz, mid, NULL);
				OutputDebugStream(L"System.gc()");
			}
			else {
				MessageBox(NULL, L"System.gc()が出来ない。", L"System.gc()が出来ない。", NULL);
			}
		}
		else {
			MessageBox(NULL, L"System.gc()が出来ない。", L"System.gc()が出来ない。", NULL);
		}

	}
}

void CJavaVMEngine::DestroyVM() {
	GC();

	// DestroyJavaVMするとバグるので何もしない
	return;

	//-------------------------------------------------------------------------
	// エンジンを破棄してから…
	//-------------------------------------------------------------------------
	if (jvm) {
		// JVM破棄
		jvm->DestroyJavaVM();
		env = NULL;
	}

	//-------------------------------------------------------------------------
	// dllを破棄。
	//-------------------------------------------------------------------------
	if (hJavaVMDll) {
		FreeLibrary(hJavaVMDll);
		hJavaVMDll = NULL;
	}
}


bool CJavaVMEngine::IsValid() {
	return env != NULL;
}


bool CJavaVMEngine::CallStaticEntryMethod(wstring class_name, wstring method_name, string arg_string, string java_args_types) {

	// utf16→utf8への変換
	string utf8_class_name = utf16_to_utf8(class_name);
	string utf8_method_name = utf16_to_utf8(method_name);

	// Helloクラスのロード
	jclass clazz = env->FindClass(utf8_class_name.c_str());
	if (clazz == 0) {
		wstring message = wstring(L"FindClass Error for `") + class_name + wstring(L"`");
		MessageBox(NULL, message.c_str(), message.c_str(), NULL);
		return false;
	}
	// 指定クラスの指定メソッド取得
	// http://setohide.blogspot.com/2014/01/jni.html
	jmethodID mid = env->GetStaticMethodID(clazz, utf8_method_name.c_str(), arg_string.c_str());
	if (mid == 0) {
		mid = env->GetStaticMethodID(clazz, utf8_method_name.c_str(), "([Ljava/lang/String;)V");
		if (mid == 0) {
			wstring message = wstring(L"GetStaticMethodID Error for `static void ") + method_name + wstring(L"(String args[])`") + L" or " + method_name + utf8_to_utf16(java_args_types);
			MessageBox(NULL, message.c_str(), message.c_str(), NULL);
			return false;
		}
	}
	// mainメソッド実行
	env->CallStaticVoidMethod(clazz, mid, NULL);

	wstring errormsg = GetErrorMessage();
	if (errormsg.size() > 0) {
		OutputDebugStream(errormsg);
	}
	GC();

	return true;
}

