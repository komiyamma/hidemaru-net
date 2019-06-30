/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#include <string>
#include <regex>
#include <tchar.h>

using namespace std;

namespace std {
#ifdef _UNICODE
	typedef std::wstring tstring;
	typedef std::wregex tregex;
	typedef std::wsmatch tsmatch;
	typedef std::wcmatch tcmatch;
#else
	typedef std::string tstring;
	typedef std::regex tregex;
	typedef std::smatch tsmatch;
	typedef std::cmatch tcmatch;
#endif
}