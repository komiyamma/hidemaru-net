/*
 * Copyright (C) 2017 Akitsugu Komiyama
 * under the MIT License
 */ 

#include <vector>
#include <windows.h>
#include <msclr/marshal_cppstd.h>

#include "convert_string.h"

std::wstring utf8_to_utf16(const std::string &text)
{
	int size = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, NULL, 0);
	std::vector<wchar_t> buf(size);
	size = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, &buf[0], (int)buf.size());
	return std::wstring(buf.begin(), buf.begin() + size).data(); // このdata()必要
}

std::string utf16_to_utf8(const std::wstring &text)
{
	int size = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, NULL, 0, 0, NULL);
	std::vector<char> buf(size);
	size = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, &buf[0], (int)buf.size(), 0, NULL);
	return std::string(buf.begin(), buf.begin() + size).data(); // このdata()必要
}

std::wstring cp932_to_utf16(const std::string &text)
{
	int size = MultiByteToWideChar(CP_OEMCP, 0, text.c_str(), -1, NULL, 0);
	std::vector<wchar_t> buf(size);
	size = MultiByteToWideChar(CP_OEMCP, 0, text.c_str(), -1, &buf[0], (int)buf.size());
	return std::wstring(buf.begin(), buf.begin() + size).data(); // このdata()必要
}

std::string utf16_to_cp932(const std::wstring &text)
{
	int size = WideCharToMultiByte(CP_OEMCP, 0, text.c_str(), -1, NULL, 0, 0, NULL);
	std::vector<char> buf(size);
	size = WideCharToMultiByte(CP_OEMCP, 0, text.c_str(), -1, &buf[0], (int)buf.size(), 0, NULL);
	return std::string(buf.begin(), buf.begin() + size).data(); // このdata()必要
}

std::wstring String_to_wstring(System::String^ str) {
	std::wstring native_string = msclr::interop::marshal_as<std::wstring>(str);
	return native_string;
}

// 「tstring」⇒「System::String^」へ。C++のtstring文字列を .NETのString^文字列へ
System::String^ wstring_to_String(std::wstring str) {
	System::String^ managed_string = msclr::interop::marshal_as<System::String^>(str);
	return managed_string;
}
