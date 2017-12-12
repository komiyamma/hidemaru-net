/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#include <windows.h>
#include <tchar.h>
#include <sstream>

#include "convert_string.h"

void OutputDebugStream(wstring wstr) {
	string cp932_data = utf16_to_cp932(wstr);

	// 長い文字は改行で区切って出しておく。
	// デバッグモニターの性質を考えると、この方がなにかと良い。
	std::stringstream is(cp932_data);
	string t;
	while (getline(is, t))
	{
		t.erase(t.find_last_not_of("\r") + 1);
		OutputDebugStringA(t.data());
		OutputDebugStringA("\n");
	}
}

