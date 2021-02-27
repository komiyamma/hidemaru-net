/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#pragma once

#include <string>

using namespace std;

// cp932Å®utf16Ç÷
std::wstring cp932_to_utf16(const std::string &text);

// utf16Å®cp932Ç÷
std::string utf16_to_cp932(const std::wstring &text);

