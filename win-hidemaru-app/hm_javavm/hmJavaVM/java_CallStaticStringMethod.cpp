#include <jni.h>
#include <string>


std::string callStaticStringMethod(JNIEnv* _env, const char* className, const char* methodName, int intArg) {
	std::string result{};

	auto clazz = _env->FindClass(className);
	auto methodID = _env->GetStaticMethodID(clazz, methodName, "(I)Ljava/lang/String;");
	if (methodID != nullptr) {
		auto jstr = static_cast<jstring>(_env->CallStaticObjectMethod(clazz, methodID, intArg));
		const char *chars = _env->GetStringUTFChars(jstr, nullptr);
		result = chars;

		_env->ReleaseStringUTFChars(jstr, chars);
		_env->DeleteLocalRef(jstr);
	}

	_env->DeleteLocalRef(clazz);
	return result;
}