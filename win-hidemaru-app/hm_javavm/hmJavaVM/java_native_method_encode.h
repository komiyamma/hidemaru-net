#pragma once

#include <windows.h>
#include <string>


struct TEncodingParam {
	int ms_codepage;
	int hm_encode;
	std::wstring java_encoding_alias;
};

extern TEncodingParam _GetEncodingParamFromHmEncode(const int hm_encode);
