/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#include <vector>
#include <windows.h>

#include "string_converter.h"


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
