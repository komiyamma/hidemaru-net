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
	// jvm.dll‚ÌJava Virtual MachineBJavaŒ¾Œê‚É“Á’è‚³‚ê‚È‚¢AJava Virtual Machine‚»‚Ì‚à‚ÌB
	static const int jni_version = JNI_VERSION_1_8;

	// jvm.dll‚Ìƒnƒ“ƒhƒ‹
	static HMODULE hJavaVMDll;
private:
	// hJavaVMDLL‚Ìƒtƒ‹ƒpƒX
	static wstring GetHavaVMDllPath();
	static wstring GetHavaVMDllPathFromSettingsFile();
private:
	// "-Djava.class.path=C:\\usr\\hidemaru;C:\\usr\\hidemaru\\hmJavaVM.jar"; // ‚Æ‚¢‚¤Œ`‚É‚·‚éB
	static char szJavaClassPathBuffer[512];


	static JavaVM *jvm;
	static JNIEnv* env;

public:
	static void CreateVM();
	static void DestroyVM();

public:
	//-------------------------------------------------------------------------
	// Java Virtual Machine‚Í“Ç‚İ‚İÏ‚İ‚Å‚©‚Â—LŒø‚©‚Ç‚¤‚©B
	static bool IsValid();
public:

	static bool HmCalled;

	// ŠY“–ƒNƒ‰ƒX‚ÌŠY“–staticƒƒ\ƒbƒh‚ğÀs‚µ‚Ä‚İ‚æ‚¤B
	static bool CallStaticEntryMethod(wstring class_name, wstring method_name);

	// ---------- LONG ‚ğ•Ô‚· --------------
	// ˆø”‚È‚µA‚©‚ÂƒfƒtƒHƒ‹ƒg‚Ì”’l•Ô‚µ i‰ß‹‚ÌŒİŠ·‚Ì‚½‚ß‚É static void funcname(String[] args) Œ^‚ª‘f‚ÅŒÄ‚Ño‚¹‚é
	static jlong CallStaticEntryMethodOfLong(wstring class_name, wstring method_name, string method_args_typedef_string = "([Ljava / lang / String;)V", string method_args_declare_string = "String[]");
	// ”’l‚Ìˆø”‚P‚Â
	static jlong CallStaticEntryMethodOfLong(wstring class_name, wstring method_name, jlong a1, string method_args_typedef_string, string method_args_declare_string);
	// •¶š—ñ‚Ìˆø”‚P‚Â
	static jlong CallStaticEntryMethodOfLong(wstring class_name, wstring method_name, wstring s1, string method_args_typedef_string, string method_args_declare_string);
	// ”’l‚Ìˆø”‚Q‚Â
	static jlong CallStaticEntryMethodOfLong(wstring class_name, wstring method_name, jlong a1, jlong a2, string method_args_typedef_string, string method_args_declare_string);
	// •¶š—ñ‚P‚Â‚ÆA”’l‚P‚Â
	static jlong CallStaticEntryMethodOfLong(wstring class_name, wstring method_name, wstring s1, jlong a2, string method_args_typedef_string, string method_args_declare_string);
	// ”’l‚P‚Â‚Æ•¶š—ñ‚P‚Â
	static jlong CallStaticEntryMethodOfLong(wstring class_name, wstring method_name, jlong a1, wstring s2, string method_args_typedef_string, string method_args_declare_string);
	// •¶š—ñ‚P‚Â‚Æ•¶š—ñ‚Q‚Â
	static jlong CallStaticEntryMethodOfLong(wstring class_name, wstring method_name, wstring s1, wstring s2, string method_args_typedef_string, string method_args_declare_string);
	// ”’l‚Ìˆø”‚R‚Â
	static jlong CallStaticEntryMethodOfLong(wstring class_name, wstring method_name, jlong a1, jlong a2, jlong a3, string method_args_typedef_string, string method_args_declare_string);
	// •¶š—ñ‚P‚Â‚Æ•¶š—ñ‚Q‚Â‚Æ•¶š—ñ‚R‚Â
	static jlong CallStaticEntryMethodOfLong(wstring class_name, wstring method_name, wstring s1, wstring s2, wstring s3, string method_args_typedef_string, string method_args_declare_string);

	// ˆÈ‰º‚R‚Â‚Í•¶š—ñ‡ŒJ‚è
	static jlong CallStaticEntryMethodOfLong(wstring class_name, wstring method_name, wstring s1, jlong a2, jlong a3, string method_args_typedef_string, string method_args_declare_string);
	static jlong CallStaticEntryMethodOfLong(wstring class_name, wstring method_name, jlong a1, wstring s2, jlong a3, string method_args_typedef_string, string method_args_declare_string);
	static jlong CallStaticEntryMethodOfLong(wstring class_name, wstring method_name, jlong a1, jlong a2, wstring s3, string method_args_typedef_string, string method_args_declare_string);

	// ˆÈ‰º‚R‚Â‚Í”’l‡ŒJ‚è
	static jlong CallStaticEntryMethodOfLong(wstring class_name, wstring method_name, jlong a1, wstring s2, wstring s3, string method_args_typedef_string, string method_args_declare_string);
	static jlong CallStaticEntryMethodOfLong(wstring class_name, wstring method_name, wstring s1, jlong a2, wstring s3, string method_args_typedef_string, string method_args_declare_string);
	static jlong CallStaticEntryMethodOfLong(wstring class_name, wstring method_name, wstring s1, wstring s2, jlong a3, string method_args_typedef_string, string method_args_declare_string);

	// ---------- STRING ‚ğ•Ô‚· --------------
	// ˆø”‚È‚µ
	static wstring CallStaticEntryMethodOfString(wstring class_name, wstring method_name, string method_args_typedef_string, string method_args_declare_string);
	// ”’l‚Ìˆø”‚P‚Â
	static wstring CallStaticEntryMethodOfString(wstring class_name, wstring method_name, jlong a1, string method_args_typedef_string, string method_args_declare_string);
	// •¶š—ñ‚Ìˆø”‚P‚Â
	static wstring CallStaticEntryMethodOfString(wstring class_name, wstring method_name, wstring s1, string method_args_typedef_string, string method_args_declare_string);
	// ”’l‚Ìˆø”‚Q‚Â
	static wstring CallStaticEntryMethodOfString(wstring class_name, wstring method_name, jlong a1, jlong a2, string method_args_typedef_string, string method_args_declare_string);
	// •¶š—ñ‚P‚Â‚ÆA”’l‚P‚Â
	static wstring CallStaticEntryMethodOfString(wstring class_name, wstring method_name, wstring a1, jlong a2, string method_args_typedef_string, string method_args_declare_string);
	// ”’l‚P‚Â‚ÆA•¶š—ñ‚P‚Â
	static wstring CallStaticEntryMethodOfString(wstring class_name, wstring method_name, jlong a1, wstring s2, string method_args_typedef_string, string method_args_declare_string);
	// •¶š—ñ‚P‚ÂA•¶š—ñ‚Q‚Â
	static wstring CallStaticEntryMethodOfString(wstring class_name, wstring method_name, wstring a1, wstring s2, string method_args_typedef_string, string method_args_declare_string);
	// ”’l‚Ìˆø”‚R‚Â
	static wstring CallStaticEntryMethodOfString(wstring class_name, wstring method_name, jlong a1, jlong a2, jlong a3, string method_args_typedef_string, string method_args_declare_string);
	// •¶š—ñ‚P‚ÂA•¶š—ñ‚Q‚ÂA•¶š—ñ‚R‚Â
	static wstring CallStaticEntryMethodOfString(wstring class_name, wstring method_name, wstring a1, wstring s2, wstring s3, string method_args_typedef_string, string method_args_declare_string);

	// ˆÈ‰º‚R‚Â‚Í•¶š—ñ‡ŒJ‚è
	static wstring CallStaticEntryMethodOfString(wstring class_name, wstring method_name, wstring s1, jlong a2, jlong a3, string method_args_typedef_string, string method_args_declare_string);
	static wstring CallStaticEntryMethodOfString(wstring class_name, wstring method_name, jlong a1, wstring s2, jlong a3, string method_args_typedef_string, string method_args_declare_string);
	static wstring CallStaticEntryMethodOfString(wstring class_name, wstring method_name, jlong a1, jlong a2, wstring s3, string method_args_typedef_string, string method_args_declare_string);

	// ˆÈ‰º‚R‚Â‚Í”’l‡ŒJ‚è
	static wstring CallStaticEntryMethodOfString(wstring class_name, wstring method_name, jlong a1, wstring s2, wstring s3, string method_args_typedef_string, string method_args_declare_string);
	static wstring CallStaticEntryMethodOfString(wstring class_name, wstring method_name, wstring s1, jlong a2, wstring s3, string method_args_typedef_string, string method_args_declare_string);
	static wstring CallStaticEntryMethodOfString(wstring class_name, wstring method_name, wstring s1, wstring s2, jlong a3, string method_args_typedef_string, string method_args_declare_string);

	static void GC();

private:
	static wstring GetErrorMessage();
};


