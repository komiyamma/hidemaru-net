#pragma once

#include <vector>
#include <map>
#include <windows.h>

#include "oniguruma/oniguruma.h"

using namespace std;

/*
	Matches matches;
	if ( PregMatch("”\—Í‚Æ•\Œ»", "(”\)(.+)(Œ»)", &matches) ) {
		OutputDebugStream << "0:" << matches[0] << ",1:" << matches[1] << ",2:" << matches[2] << endl;
	}		
 */
using Matches = map<int, string>;

int OnigMatch(string str_target, string str_regex, Matches* pmatches = NULL);