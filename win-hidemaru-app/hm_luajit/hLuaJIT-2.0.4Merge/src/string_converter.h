#pragma once

#include <string>

std::wstring utf8_to_utf16(const std::string &text);
std::string utf16_to_utf8(const std::wstring &text);
