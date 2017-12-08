/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the MIT License
 */

#pragma once

#include <string>

using namespace std;

// utf8Å®utf16Ç÷
std::wstring utf8_to_utf16(const std::string &text);

// utf16Å®utf8Ç÷
std::string utf16_to_utf8(const std::wstring &text);

// cp932Å®utf16Ç÷
std::wstring cp932_to_utf16(const std::string &text);

// utf16Å®cp932Ç÷
std::string utf16_to_cp932(const std::wstring &text);

