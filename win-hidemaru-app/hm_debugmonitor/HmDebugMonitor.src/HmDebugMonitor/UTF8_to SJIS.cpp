#include <windows.h>
#include <string>
#include <iostream>
#include <vector>

// const char *text -> const std::string &textÇ…ïœçXÇ»Ç«ínñ°Ç…èCê≥

std::wstring to_WideChar(UINT uCodePage, const std::string &text)
{
	int size=MultiByteToWideChar(uCodePage, 0, text.c_str(), -1, NULL, 0);
	std::vector<wchar_t> buf(size);
	size=MultiByteToWideChar(uCodePage, 0, text.c_str(), -1, &buf[0], buf.size());
	return std::wstring(buf.begin(), buf.begin()+size);
}

std::string to_MultiByte(UINT uCodePage, const std::wstring &text)
{
	int size=WideCharToMultiByte(uCodePage, 0, text.c_str(), -1, NULL, 0, 0, NULL);
	std::vector<char> buf(size);
	size=WideCharToMultiByte(uCodePage, 0, text.c_str(), -1, &buf[0], buf.size(), 0, NULL);
	return std::string(buf.begin(), buf.begin()+size);
}

std::string cp932_to_utf8(const std::string &text)
{
	return to_MultiByte(CP_UTF8, to_WideChar(CP_OEMCP, text));
}


std::string utf8_to_cp932(const std::string &text)
{
	return to_MultiByte(CP_OEMCP, to_WideChar(CP_UTF8, text));
}

