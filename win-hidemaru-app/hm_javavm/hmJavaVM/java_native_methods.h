#pragma once

#include <jni.h>

extern "C" JNIEXPORT jstring JNICALL Java_Hm_concat(JNIEnv *, jobject, jstring, jstring);
extern "C" JNIEXPORT jstring JNICALL Java_Hm_MacroVarGetObj(JNIEnv *, jobject, jstring);
