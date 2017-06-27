#include <jni.h>
#include <windows.h>
#include <string>
#include <iostream>

#include "convert_string.h"
#include "hidemaruexe_export.h"
#include "output_debugstream.h"
#include "self_dll_info.h"
#include "dllfunc_interface.h"
#include "dllfunc_interface_internal.h"

// javastring → utf16へ
std::wstring jstr_to_utf16(JNIEnv *env, jstring& jtext) {
	const char *utf8_text = env->GetStringUTFChars(jtext, 0);
	wstring utf16_text = utf8_to_utf16(utf8_text);
	env->ReleaseStringUTFChars(jtext, utf8_text);
	return utf16_text;
}


// 秀丸マクロのシンボルや変数の値の取得。
JNIEXPORT jstring JNICALL Java_Hm_MacroVarGetObj(JNIEnv *env, jobject obj, jstring key_name) {

	TestDynamicVar.Clear();
	auto dll_invocant = CSelfDllInfo::GetInvocantString();

	wstring utf16_key_name = jstr_to_utf16(env, key_name);

	wstring cmd = L"##_tmp_dll_id_ret = dllfuncw( " + dll_invocant + L"\"SetDynamicVar\", " + utf16_key_name + L");\n"
		L"##_tmp_dll_id_ret = 0;\n";
	BOOL success = CHidemaruExeExport::EvalMacro(cmd);

	// 数値なら
	if (TestDynamicVar.type == CDynamicValue::TDynamicType::TypeInteger)
	{
		wstring return_value = L"";
		if (sizeof(intptr_t) == sizeof(int)) {
			return_value = wstring(L"HmJavaVMType_Int32<<>>") + std::to_wstring(TestDynamicVar.num);
		}
		else {
			return_value = wstring(L"HmJavaVMType_Int64<<>>") + std::to_wstring(TestDynamicVar.num);
		}
		string utf8_return_value = utf16_to_utf8(return_value);
		return env->NewStringUTF(utf8_return_value.c_str());
	}
	// 文字列なら
	else {
		wstring return_value = wstring(L"HmJavaVMType_String<<>>") + TestDynamicVar.wstr;
		string utf8_return_value = utf16_to_utf8(return_value);
		return env->NewStringUTF(utf8_return_value.c_str());
	}
}

// 秀丸マクロのシンボルや変数へ値を設定。数値系。
JNIEXPORT jboolean JNICALL Java_Hm_MacroVarSetNum(JNIEnv *env, jobject obj, jstring key_name, jlong value) {
	jboolean success = 0;

	auto dll_invocant = CSelfDllInfo::GetInvocantString();

	wstring utf16_key_name = jstr_to_utf16(env, key_name);

	wchar_t start = utf16_key_name[0];

	if (start == L'#') {

		PushNumVar((intHM_t)value);
		wstring cmd = L" = dllfuncw( " + dll_invocant + L"\"PopNumVar\" );\n";
		cmd = utf16_key_name + cmd;
		success = CHidemaruExeExport::EvalMacro(cmd);
	}
	else {
		success = false;
	}

	return success;
}

// 秀丸マクロのシンボルや変数へ値を設定。文字列系。
JNIEXPORT jboolean JNICALL Java_Hm_MacroVarSetStr(JNIEnv *env, jobject obj, jstring key_name, jstring value) {
	jboolean success = 0;

	auto dll_invocant = CSelfDllInfo::GetInvocantString();

	wstring utf16_key_name = jstr_to_utf16(env, key_name);

	wstring utf16_value = jstr_to_utf16(env, value);

	wchar_t start = utf16_key_name[0];

	if (start == L'$') {

		PushStrVar(utf16_value.data());
		wstring cmd = L" = dllfuncstrw( " + dll_invocant + L"\"PopStrVar\" );\n";
		cmd = utf16_key_name + cmd;
		success = CHidemaruExeExport::EvalMacro(cmd);
	}
	else {
		success = false;
	}

	return success;
}

// 秀丸マクロのシンボルや変数へ値を設定。文字列系。
JNIEXPORT void JNICALL Java_Hm_DebugInfo(JNIEnv *env, jobject obj, jstring value) {

	wstring utf16_value = jstr_to_utf16(env, value);

	OutputDebugStream(utf16_value);
}