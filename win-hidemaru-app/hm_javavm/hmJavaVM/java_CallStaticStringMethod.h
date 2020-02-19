#pragma once
#include <jni.h>
#include <string>

extern std::string CallStaticStringMethod(JNIEnv* env, const char* className, const char* methodName, int intArg);

