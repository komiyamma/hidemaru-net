/* 
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the MIT License
 */

#include <stdlib.h>
#include <math.h>

#define lib_print_ex_c
#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

#include <windows.h>

#include "outputdebugstring.h"

LUA_API int luaB_dprint (lua_State *L) {
	int n = lua_gettop(L);  /* number of arguments */
	int i;
	lua_getglobal(L, "tostring");
	for (i=1; i<=n; i++) {
		const char *s;
		lua_pushvalue(L, -1);  /* function to be called */
		lua_pushvalue(L, i);   /* value to print */
		lua_call(L, 1, 1);
		s = lua_tostring(L, -1);  /* get result */
		if (s == NULL)
			return luaL_error(L, LUA_QL("tostring") " must return a string to "
			LUA_QL("print"));
		if (i>1) ODSfputs("\t", stdout);
		ODSfputs(s, stdout);
		lua_pop(L, 1);  /* pop result */
	}
	ODSfputs("\n", stdout);
	return 0;
}


static const luaL_Reg hidemaru_ex_lib[] = {
	{"debuginfo", luaB_dprint},
	{NULL, NULL}
};


/*
** Open math library
*/
LUALIB_API int luaopen_hidemaru_ex (lua_State *L) {
	luaL_register(L, "hm", hidemaru_ex_lib);
	return 1;
}




#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


size_t ODSfwrite(const void *buf, size_t size, size_t n, FILE *fp) {
	size_t ret=fwrite(buf, size, n, fp);
	if ( fp == stderr || fp == stdout ) {
		char szBufDebug[4096] = "";
	    int length = sizeof(szBufDebug);
		strncpy(szBufDebug, buf, length);
	    szBufDebug[length - 1] = '\0';
		OutputDebugString(szBufDebug);
	}
	return ret;
}


int ODSfputs( const char *str , FILE *fp ) {
	int ret=fputs(str, fp);
	if ( fp == stderr || fp == stdout ) {
		char szBufDebug[4096] = "";
	    int length = sizeof(szBufDebug);
		strncpy(szBufDebug, str, length);
	    szBufDebug[length - 1] = '\0';
		OutputDebugString(szBufDebug);
	}
	return ret;
}

int ODSputs( const char *str ) {
	int ret =  puts(str);
	char szBufDebug[4096] = "";
    int length = sizeof(szBufDebug);
	strncpy(szBufDebug, str, length);
    szBufDebug[length - 1] = '\0';
	OutputDebugString(szBufDebug);
	return ret;
}

int ODSputc( int c, FILE *stream ) {
	char buf[2] = "";
	buf[0] = (unsigned char)c;
	if ( stream == stderr || stream == stdout ) {
		OutputDebugString(buf);
	}
	return putc(c, stream);
}

int ODSfprintf( FILE *stream, char *format, ... ) {
	char szBufDebug[4096] = "";
	int ret = 0;
	int length = sizeof(szBufDebug);

	va_list arg;

	va_start(arg, format);
	ret = vfprintf( stream, format, arg );
	_vsnprintf_s(szBufDebug, length, _TRUNCATE, format, arg);
	va_end(arg);

	szBufDebug[length - 1] = '\0';

	if ( stream == stderr || stream == stdout ) {
		OutputDebugString( szBufDebug );
	}
	return ret;
}

int ODSprintf( char *format, ... ) {
	char szBufDebug[4096] = "";
	int ret = 0;
	int length = sizeof(szBufDebug);

	va_list arg;

	va_start(arg, format);
	ret = vprintf( format, arg );
	_vsnprintf_s(szBufDebug, length, _TRUNCATE, format, arg);
	va_end(arg);

	szBufDebug[length - 1] = '\0';

	OutputDebugString( szBufDebug );
	return ret;
}
