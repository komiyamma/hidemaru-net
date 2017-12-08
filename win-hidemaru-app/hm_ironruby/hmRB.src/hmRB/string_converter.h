/* 
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */


#pragma once

#include <string>
#include <tchar.h>

namespace std {
	using tstring = std::basic_string<TCHAR>;
}

System::String^ tstring_to_String(std::tstring str);
std::tstring String_to_tstring(System::String^ str);