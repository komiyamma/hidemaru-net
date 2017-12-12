/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#include "java_vm_engine.h"
#include "convert_string.h"
#include "output_debugstream.h"


// エラー文字列の形成。JNIにはエラー文字列を形成するための関数は備わっていないため、こねこね
static void _append_exception_trace_messages(
	JNIEnv&      a_jni_env,
	std::wstring& a_error_msg,
	jthrowable   a_exception,
	jmethodID    a_mid_throwable_getCause,
	jmethodID    a_mid_throwable_getStackTrace,
	jmethodID    a_mid_throwable_toString,
	jmethodID    a_mid_frame_toString)
{
	// Get the array of StackTraceElements.
	jobjectArray frames =
		(jobjectArray)a_jni_env.CallObjectMethod(
		a_exception,
		a_mid_throwable_getStackTrace);
	jsize frames_length = a_jni_env.GetArrayLength(frames);

	// 降順にする前にThrowable.toString（）を追加する
	//トレースメッセージをスタックする。
	if (frames != 0)
	{
		jstring msg_obj = (jstring)a_jni_env.CallObjectMethod(a_exception, a_mid_throwable_toString);
		const char* msg_str = a_jni_env.GetStringUTFChars(msg_obj, 0);

		// If this is not the top-of-the-trace then
		// this is a cause.
		if (!a_error_msg.empty())
		{
			a_error_msg += L"\nCaused by: ";
			a_error_msg += utf8_to_utf16(msg_str);
		}
		else
		{
			a_error_msg = utf8_to_utf16(msg_str);
		}

		a_jni_env.ReleaseStringUTFChars(msg_obj, msg_str);
		a_jni_env.DeleteLocalRef(msg_obj);
	}

	//スタックトレースメッセージがあれば追加。
	if (frames_length > 0)
	{
		jsize i = 0;
		for (i = 0; i < frames_length; i++)
		{
			// Get the string returned from the 'toString()'
			// method of the next frame and append it to
			// the error message.
			jobject frame = a_jni_env.GetObjectArrayElement(frames, i);
			jstring msg_obj = (jstring)a_jni_env.CallObjectMethod(frame, a_mid_frame_toString);

			const char* msg_str = a_jni_env.GetStringUTFChars(msg_obj, 0);

			a_error_msg += L"\n    ";
			a_error_msg += utf8_to_utf16(msg_str);

			a_jni_env.ReleaseStringUTFChars(msg_obj, msg_str);
			a_jni_env.DeleteLocalRef(msg_obj);
			a_jni_env.DeleteLocalRef(frame);
		}
	}

	// 'a_exception'に原因がある場合は、
	// そのトレース・メッセージをスタックする。
	if (frames != 0)
	{
		jthrowable cause = (jthrowable)a_jni_env.CallObjectMethod(a_exception, a_mid_throwable_getCause);
		if (cause != 0)
		{
			_append_exception_trace_messages(a_jni_env,
				a_error_msg,
				cause,
				a_mid_throwable_getCause,
				a_mid_throwable_getStackTrace,
				a_mid_throwable_toString,
				a_mid_frame_toString);
		}
	}
}

wstring CJavaVMEngine::GetErrorMessage() {
	jthrowable exception = env->ExceptionOccurred();
	if (exception != NULL) {

		env->ExceptionClear();

		jclass throwable_class = env->FindClass("java/lang/Throwable");

		jmethodID mid_throwable_getCause =
			env->GetMethodID(throwable_class,
			"getCause",
			"()Ljava/lang/Throwable;");
		jmethodID mid_throwable_getStackTrace =
			env->GetMethodID(throwable_class,
			"getStackTrace",
			"()[Ljava/lang/StackTraceElement;");
		jmethodID mid_throwable_toString =
			env->GetMethodID(throwable_class,
			"toString",
			"()Ljava/lang/String;");

		wstring error_msg = L""; // Could use ostringstream instead.
		_append_exception_trace_messages(*env, error_msg, exception, mid_throwable_getCause, mid_throwable_getStackTrace, mid_throwable_toString, mid_throwable_toString);

		return error_msg;
	}
	else {
		return L"";
	}
}
