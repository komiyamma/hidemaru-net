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

// lua�C���^�v���^���[�h
void LoadLuaInterpreter();
// lua�C���^�v���^���
void FreeLuaInterpreter();

// lua_getglobal�̐��l�^�C�v�̃��b�p�[
int GetNum(const char *var_name);

// lua_getglobal�̕�����^�C�v�̃��b�p�[
const char* GetStr(const char *var_name);

int DoString(const char *command);

