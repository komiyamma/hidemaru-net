/* 
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the MIT License
 */

#include <stddef.h>

#define lib_tab_ex_c
#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

/*
 ** Structure with table-access functions 
 */ 
typedef struct { 
	int (*geti) (lua_State *L, int idx, lua_Integer n); 
	void (*seti) (lua_State *L, int idx, lua_Integer n); 
} TabA; 

static int lua_absindex (lua_State *L, int i) { 
	if (i < 0 && i > LUA_REGISTRYINDEX) 
		i += lua_gettop(L) + 1; 
	return i; 
}

static int lua_geti (lua_State *L, int index, lua_Integer i) { 
	index = lua_absindex(L, index); 
	lua_pushinteger(L, i); 
	lua_gettable(L, index); 
	return lua_type(L, -1); 
}
 
static void lua_seti (lua_State *L, int index, lua_Integer i) { 
	luaL_checkstack(L, 1, "not enough stack slots available"); 
	index = lua_absindex(L, index); 
	lua_pushinteger(L, i); 
	lua_insert(L, -2); 
	lua_settable(L, index); 
} 

#pragma warning( push )
#pragma warning(disable:4133)  // warning C4133: '=' : 'void (__cdecl *)(lua_State *,int,int)' ‚Æ 'int (__cdecl *)(lua_State *,int,lua_Integer)' ‚ÌŠÔ‚ÅŒ^‚ÉŒÝŠ·«‚ª‚ ‚è‚Ü‚¹‚ñB
static int tmove (lua_State *L) { 
	TabA ta; 
	lua_Integer f = luaL_checkinteger(L, 2); 
	lua_Integer e = luaL_checkinteger(L, 3); 
	lua_Integer t = luaL_checkinteger(L, 4); 
	int tt = !lua_isnoneornil(L, 5) ? 5 : 1;  /* destination table */ 
	/* the following restriction avoids several problems with overflows */ 
	luaL_argcheck(L, f > 0, 2, "initial position must be positive"); 
	if (e >= f) {  /* otherwise, nothing to move */ 
		lua_Integer n, i; 
		ta.geti = (luaL_getmetafield(L, 1, "__index") == LUA_TNIL) ?
					(luaL_checktype(L, 1, LUA_TTABLE), lua_rawgeti) : lua_geti; 
		ta.seti = (luaL_getmetafield(L, tt, "__newindex") == LUA_TNIL) ?
					(luaL_checktype(L, tt, LUA_TTABLE), lua_rawseti) : lua_seti; 
		n = e - f + 1;  /* number of elements to move */ 
		if (t > f) { 
			for (i = n - 1; i >= 0; i--) { 
				(*ta.geti)(L, 1, f + i); 
				(*ta.seti)(L, tt, t + i); 
			} 
		} 
		else { 
			for (i = 0; i < n; i++) { 
				(*ta.geti)(L, 1, f + i); 
				(*ta.seti)(L, tt, t + i); 
			} 
		} 
	} 
	lua_pushvalue(L, tt);  /* return "to table" */ 
	return 1; 
} 
#pragma warning( pop )



static int tempty (lua_State *L) {
	if (lua_isnil(L, 1)) {
		lua_pushboolean(L, 1);
		return 1;
	} else {
		luaL_checktype(L, 1, LUA_TTABLE);
		lua_settop(L, 2);  /* create a 2nd argument if there isn't one */
		if (lua_next(L, 1)) {
			lua_pushboolean(L, 0);
			return 1;
		} else {
			lua_pushboolean(L, 1);
			return 1;
		}
	}
}

static const luaL_Reg tab_ex_funcs[] = {
  {"move", tmove},
  {"empty", tempty},
  {NULL, NULL}
};


LUALIB_API int luaopen_table_ex (lua_State *L) {
  luaL_register(L, LUA_TABLIBNAME, tab_ex_funcs);
  return 1;
}
