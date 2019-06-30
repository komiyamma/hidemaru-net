/* 
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#pragma once

#include <windows.h>

#define MACRO_DLL extern "C" __declspec(dllexport)


#define intHM_t intptr_t



//========================================================================
/// 他のファイルから見ているためのextern。実際は.cppの方をみてね
//========================================================================
MACRO_DLL intHM_t GetNumVar(const wchar_t *utf16_simbol);
MACRO_DLL intHM_t SetNumVar(const wchar_t *utf16_simbol, intHM_t value);
MACRO_DLL const wchar_t * GetStrVar(const wchar_t *utf16_simbol);
MACRO_DLL intHM_t SetStrVar(const wchar_t *utf16_simbol, const wchar_t *utf16_value);
