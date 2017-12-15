/* 
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#pragma once

#include "dllfunc_interface.h"
#include <string>

using namespace std;

namespace PythonEngine {

	BOOL IsValid();

	int Create();
	int Initialize();
	int DoString(wstring utf16_expressoin);
	int Destroy();

	intHM_t GetNumVar(wstring utf16_simbol);
	BOOL SetNumVar(wstring utf16_simbol, intHM_t);

	wstring GetStrVar(wstring utf16_simbol);
	BOOL SetStrVar(wstring utf16_simbol, wstring utf16_value);

	wstring GetPythonPath();
}

