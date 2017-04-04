#include <windows.h>
#include <sstream>

#include "convert_string.h"


#ifdef _DEBUG

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

void OutputDebugStream(string str) {
	OutputDebugStream(cp932_to_utf16(str));
}

#endif