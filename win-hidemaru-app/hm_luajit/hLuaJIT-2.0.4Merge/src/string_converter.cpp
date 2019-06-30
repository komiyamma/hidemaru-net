/* 
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the MIT License
 */

#include <windows.h>
#include <string>
#include <vector>

using namespace std;


std::wstring utf8_to_utf16(const std::string &text)
{
	int size = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, NULL, 0);
	std::vector<wchar_t> buf(size);
	size = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, &buf[0], buf.size());
	return std::wstring(buf.begin(), buf.begin() + size).data();
}

std::string utf16_to_utf8(const std::wstring &text)
{
	int size = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, NULL, 0, 0, NULL);
	std::vector<char> buf(size);
	size = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, &buf[0], buf.size(), 0, NULL);
	return std::string(buf.begin(), buf.begin() + size).data();
}