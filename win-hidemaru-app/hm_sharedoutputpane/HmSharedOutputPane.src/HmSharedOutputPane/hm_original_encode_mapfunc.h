#pragma once

#include <windows.h>
#include <string>
#include <vector>

// ���̌��ʂ̃o�C�g��(vector.data())�� HmOutputPane.dll��Output�֐��ɂȂ���΁AUnicode�ł��Č��ł���
std::vector<BYTE> EncodeWStringToOriginalEncodeVector(std::wstring original_string);

