/* 
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the MIT License
 */

#pragma once

#include <string>
#include <windows.h>

#include "lua.hpp"

#include "OutputDebugStream.h"

using namespace std;

extern lua_State* L;
extern int lua_State_Err;

// luaインタプリタロード
void LoadLuaInterpreter();
// luaインタプリタ解放
void FreeLuaInterpreter();

// lua_getglobalの数値タイプのラッパー
int GetNum(const char *var_name);

// lua_getglobalの文字列タイプのラッパー
const char* GetStr(const char *var_name);

int DoString(const char *command);

