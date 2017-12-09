/*
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#include <string>
#include <msclr/marshal_cppstd.h>
#include "string_converter.h"



// �uSystem::String^�v�ˁustd::string�v�ցB.NET��String^������� C++��string�������
std::tstring String_to_tstring(System::String^ str) {
	std::wstring native_string = msclr::interop::marshal_as<std::tstring>(str);
	return native_string;
}

// �utstring�v�ˁuSystem::String^�v�ցBC++��tstring������� .NET��String^�������
System::String^ tstring_to_String(std::tstring str) {
	System::String^ managed_string = msclr::interop::marshal_as<System::String^>(str);
	return managed_string;
}

