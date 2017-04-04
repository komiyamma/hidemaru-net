#pragma once

#include <string>

std::wstring cp932_to_utf16(const std::string& str);
std::string utf16_to_cp932(const std::wstring& str);