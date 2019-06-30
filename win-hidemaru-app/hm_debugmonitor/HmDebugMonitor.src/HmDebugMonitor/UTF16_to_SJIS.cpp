/*
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the MIT License
 */

#include <string>
#include <windows.h>
#include <cstring>
std::wstring cp932_to_utf16(const std::string& str)
{
	static_assert(sizeof(wchar_t) == 2, "this functon is windows only");
	const int len = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, nullptr, 0);
	std::wstring re(len * 2 + 2, L'\0');
	if (!::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, &re[0], len)) {
		const auto ec = ::GetLastError();
		switch (ec)
		{
		case ERROR_INSUFFICIENT_BUFFER:
			throw std::runtime_error("in function utf_16_to_shift_jis, WideCharToMultiByte fail. cause: ERROR_INSUFFICIENT_BUFFER"); break;
		case ERROR_INVALID_FLAGS:
			throw std::runtime_error("in function utf_16_to_shift_jis, WideCharToMultiByte fail. cause: ERROR_INVALID_FLAGS"); break;
		case ERROR_INVALID_PARAMETER:
			throw std::runtime_error("in function utf_16_to_shift_jis, WideCharToMultiByte fail. cause: ERROR_INVALID_PARAMETER"); break;
		case ERROR_NO_UNICODE_TRANSLATION:
			throw std::runtime_error("in function utf_16_to_shift_jis, WideCharToMultiByte fail. cause: ERROR_NO_UNICODE_TRANSLATION"); break;
		default:
			throw std::runtime_error("in function utf_16_to_shift_jis, WideCharToMultiByte fail. cause: unknown(" + std::to_string(ec) + ')'); break;
		}
	}
	const std::size_t real_len = std::wcslen(re.c_str());
	re.resize(real_len);
	re.shrink_to_fit();
	return re;
}
std::string utf16_to_cp932(const std::wstring& str) {
	static_assert(sizeof(wchar_t) == 2, "this functon is windows only");
	const int len = ::WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, nullptr, 0, nullptr, nullptr);
	std::string re(len * 2, '\0');
	if (!::WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, &re[0], len, nullptr, nullptr)) {
		const auto ec = ::GetLastError();
		switch (ec)
		{
		case ERROR_INSUFFICIENT_BUFFER:
			throw std::runtime_error("in function utf_16_to_shift_jis, WideCharToMultiByte fail. cause: ERROR_INSUFFICIENT_BUFFER"); break;
		case ERROR_INVALID_FLAGS:
			throw std::runtime_error("in function utf_16_to_shift_jis, WideCharToMultiByte fail. cause: ERROR_INVALID_FLAGS"); break;
		case ERROR_INVALID_PARAMETER:
			throw std::runtime_error("in function utf_16_to_shift_jis, WideCharToMultiByte fail. cause: ERROR_INVALID_PARAMETER"); break;
		default:
			throw std::runtime_error("in function utf_16_to_shift_jis, WideCharToMultiByte fail. cause: unknown(" + std::to_string(ec) + ')'); break;
		}
	}
	const std::size_t real_len = std::strlen(re.c_str());
	re.resize(real_len);
	re.shrink_to_fit();
	return re;
}

