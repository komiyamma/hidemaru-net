#pragma once

#include <windows.h>
#include <string>
#include <vector>

// この結果のバイト列(vector.data())を HmOutputPane.dllのOutput関数になげれば、Unicodeでも再現できる
std::vector<BYTE> EncodeWStringToOriginalEncodeVector(std::wstring original_string);

