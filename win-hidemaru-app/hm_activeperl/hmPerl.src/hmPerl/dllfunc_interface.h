/*
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#pragma once

#include <tchar.h>
#include <cstdint>

#define MACRO_DLL extern "C" __declspec(dllexport)


#define intHM_t intptr_t



//========================================================================
/// 他のファイルから見ているためのextern。実際は.cppの方をみてね
//========================================================================
MACRO_DLL intHM_t GetNumVar(const TCHAR *sz_full_var_name);
MACRO_DLL intHM_t SetNumVar(const TCHAR *sz_full_var_name, intHM_t value);
MACRO_DLL const TCHAR * GetStrVar(const TCHAR *sz_full_var_name);
MACRO_DLL intHM_t SetStrVar(const TCHAR *sz_full_var_name, const TCHAR *value);
