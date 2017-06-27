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
JavaVM *CJavaVMEngine::jvm = NULL;
JNIEnv* CJavaVMEngine::env = NULL;


void CJavaVMEngine::CreateVM() {
	if (IsValid()) {
		return;
	}

	// 読み取った値をパスとして動的にjvm.dllライブラリーをロード
	hJavaVMDll = LoadLibrary(LR"(C:\Program Files (x86)\Java\jdk1.8.0_131\jre\bin\client\jvm.dll)");
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
	MessageBox(NULL, L"OK1", L"OK1", NULL);

	// JNI_CreateJavaVMを呼び出し
	JavaVMOption options[3];
	options[0].optionString = "-Xmx256m";
	options[1].optionString = "-Djava.class.path=.";
	JavaVMInitArgs vm_args;
	vm_args.version = jni_version;
	vm_args.options = options;
	vm_args.nOptions = 2;

	MessageBox(NULL, L"OK2", L"OK2", NULL);
	env = NULL;
	jvm = NULL;

	try {
		int isValid = pCreateJavaVM(&jvm, (void**)&env, &vm_args);
		if (isValid < 0) {
			wstring msg = L"JNI_CreateJavaVM Error: " + std::to_wstring(isValid);
			MessageBox(NULL, msg.c_str(), msg.c_str(), NULL);
			return;
		}
	}
	catch (exception e) {
		MessageBoxA(NULL, e.what(), e.what(), NULL);
	}

	MessageBox(NULL, L"OK3", L"OK3", NULL);
}


void CJavaVMEngine::DestroyVM() {

	// フリーするとバグるので何もしない
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


bool CJavaVMEngine::GetStaticActionMethod(wstring class_name, wstring method_name) {
	MessageBox(NULL, L"DoTest", L"DoTest", NULL);

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
	// Helloクラスのmainメソッド取得
	jmethodID mid = env->GetStaticMethodID(clazz, utf8_method_name.c_str(), "([Ljava/lang/String;)V" );
	if (mid == 0) {
		wstring message = wstring(L"GetStaticMethodID Error for `static void ") + method_name + wstring(L"(String args[])`");
		MessageBox(NULL, message.c_str(), message.c_str(), NULL);
		return false;
	}
	// mainメソッド実行
	env->CallStaticVoidMethod(clazz, mid, NULL);

	wstring errormsg = GetErrorMessage();
	if (errormsg.size() > 0) {
		OutputDebugStream(errormsg);
	}

	MessageBox(NULL, L"DoTest2", L"DoTest2", NULL);

	return true;
}

