/* 
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */


#include <string>
#include <msclr/marshal_cppstd.h>
#include "string_converter.h"



// 「System::String^」⇒「std::string」へ。.NETのString^文字列を C++のstring文字列へ
std::tstring String_to_tstring(System::String^ str) {
	std::wstring native_string = msclr::interop::marshal_as<std::tstring>(str);
	return native_string;
}

// 「tstring」⇒「System::String^」へ。C++のtstring文字列を .NETのString^文字列へ
System::String^ tstring_to_String(std::tstring str) {
	System::String^ managed_string = msclr::interop::marshal_as<System::String^>(str);
	return managed_string;
}

