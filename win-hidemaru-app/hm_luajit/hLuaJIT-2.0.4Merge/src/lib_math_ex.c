/* 
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the MIT License
 */

#include <stdlib.h>
#include <math.h>

#define lib_math_ex_c
#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

/**
 * sprintf関数を利用して, 任意の小数点以下を四捨五入する
 * @param[in] x 四捨五入を行う浮動小数点数
 * @param[in] precision 小数点以下の桁数
 * @return 四捨五入の結果
 */
static double _math_round(double x, int precision) {
	char s[256] = {0};

    sprintf(s, "%.*f", precision - 1, x);
    return strtod(s, NULL);
}

static int math_round (lua_State *L) {
	lua_Number x = luaL_checknumber(L, 1);
	int precision = luaL_optint(L, 2, 1);  // 2 番目の引数が数値であれば、 それを int にキャストした数値を返します。 その引数が存在しないか nil であれば、 1 を返します。 いずれでもなければエラーを発生します。 

	lua_Number d = _math_round(x, precision);

	lua_pushnumber(L, d);
	return 1;
}


static int math_type (lua_State *L) {
	if (lua_type(L, 1) == LUA_TNUMBER) {
		double d = luaL_checknumber(L, 1);
		if ( d == floor(d) ) {
			lua_pushliteral(L, "integer");
		}
		else {
			lua_pushliteral(L, "float"); 
		}
	}
	else {
		luaL_checkany(L, 1);
		lua_pushnil(L);
	}
	return 1;
}


static int math_tointeger (lua_State *L) {
	if (lua_type(L, 1) == LUA_TNUMBER) {
		double d = luaL_checknumber(L, 1);
		double fd = floor(d);
		if ( d == fd && INT_MIN <= d && d <= INT_MAX ) {
			lua_pushnumber(L, fd);
		}
		else {
			lua_pushnil(L);
		}
	}
	else {
		luaL_checkany(L, 1);
		lua_pushnil(L);
	}
	return 1;
}


static int math_ult (lua_State *L) {
  lua_Integer a = luaL_checkinteger(L, 1);
  lua_Integer b = luaL_checkinteger(L, 2);
  lua_pushboolean(L, (unsigned long long)a < (unsigned long long)b);
  return 1;
}




static const luaL_Reg mathlib[] = {
       {"ult",   math_ult},
	{"round",  math_round},
	{"type",   math_type},
	{"tointeger", math_tointeger},
	{NULL, NULL}
};


/*
** Open math library
*/
LUALIB_API int luaopen_math_ex (lua_State *L) {
	luaL_register(L, LUA_MATHLIBNAME, mathlib);
	lua_pushinteger(L, INT_MAX);
	lua_setfield(L, -2, "maxinteger");
	lua_pushinteger(L, INT_MIN);
	lua_setfield(L, -2, "mininteger");
	return 1;
}

