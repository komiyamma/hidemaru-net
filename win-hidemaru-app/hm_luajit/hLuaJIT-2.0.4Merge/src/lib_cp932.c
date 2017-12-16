/* 
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the MIT License
 */

#define lib_cp932_c
#define LUA_LIB

#define _CRT_SECURE_NO_WARNINGS

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

#include "mbenc.h"/* multi-byte encoding support */
#include "nmh.h"

#include <windows.h>

/* macro to `unsign' a character */
#define uchar(c)        ((unsigned char)(c))



static ptrdiff_t posrelat (ptrdiff_t pos, size_t len) {
	/* relative string position: negative means back from end */
	if (pos < 0) pos += (ptrdiff_t)len + 1;
	return (pos >= 0) ? pos : 0;
}

static int str_cp932_len(lua_State *L) {
	size_t l, i, c;
	const char *s = luaL_checklstring(L, 1, &l);
	for (c = i = 0; i < l; i += mb_len(&(s[i]))) c++;
	lua_pushinteger(L, c);
	return 1;
}

#pragma warning(push)
#pragma warning(disable: 4018)
static int str_cp932_offset(lua_State *L) {
	size_t l, i, c;
	const char *s = luaL_checklstring(L, 1, &l);
	lua_Integer n  = luaL_checkinteger(L, 2);

	// ���������o���Bc���������B
	for (c = i = 0; i < l; i += mb_len(&(s[i]))) c++;
	// �w�肵�Ă���L�����N�^�����A������+1�𒴂���Ƃ��́Anil��Ԃ��B

	// n�ɕ��̐����w�肵�Ă���Ȃ�΁A������̒���+1����A����������������n�Ƃ��Ďw�肵�����ƂƓ���
	if ( n < 0 ) {
		n = c + n + 1;
	}

	if ( n <= 0 || c+1 < n ) { // 0���傫���A������+1 �܂ł��F�߂���B������+1���w�肵�Ă���ꍇ�́A�S����i�̍��v
		lua_pushnil(L);
	} else {
		// ���������o���Bc���������B
		for (c = i = 0; i < l; c++) {
			if ( c < n - 1 ) {
				i += mb_len( &(s[i]) );
			} else {
				break;
			}
		}
		lua_pushinteger(L, i+1);
	}
	return 1;
}
#pragma warning(pop)



static int str_cp932_sub(lua_State *L) {
	size_t l;
	const char *s = luaL_checklstring(L, 1, &l);
	size_t start = posrelat(luaL_checkinteger(L, 2), l);
	size_t end = posrelat(luaL_optinteger(L, 3, -1), l);
	if (start < 1) start = 1;
	if (end > l) end = l;
	if (start <= end) {
		size_t i, a = 0, b = 0, c = 0;
		for (i = 0; i < l; i += mb_len(&(s[i]))) {
			c++;
			if (c == start) a = i;
			if (c == end) b = (i + mb_len(&(s[i]))) ;
			if (a && b) break;
		}
		if (b == 0) b = end;
		lua_pushlstring(L, &(s[a]), b - a);
	} else lua_pushliteral(L, "");
	return 1;
}

static int str_cp932_reverse (lua_State *L) {
  size_t l;
  long long i;
  luaL_Buffer b;
  const char *s = luaL_checklstring(L, 1, &l);
  luaL_buffinit(L, &b);
  for (i = l - 1; i >= 0; ) {
    if ((i > 0) && 
     is_sjis_1(s[i - 1]) && is_sjis_2(s[i - 0])) {
      luaL_addchar(&b, uchar(s[i - 1]));
      luaL_addchar(&b, uchar(s[i - 0]));
      i -= 2;
    } else {
      luaL_addchar(&b, uchar(s[i]));
      i -= 1;
    }
  }
  luaL_pushresult(&b);
  return 1;
}

static int str_cp932_lower (lua_State *L) {
	size_t l;
	size_t i;
	luaL_Buffer b;
	const char *s = luaL_checklstring(L, 1, &l);
	luaL_buffinit(L, &b);
	for (i = 0; i < l;) {
		if (is_sjis_1(s[i + 0]) && is_sjis_2(s[i + 1])) {
			luaL_addchar(&b, s[i + 0]);
			luaL_addchar(&b, s[i + 1]);
			i += 2;
		} else {
			luaL_addchar(&b, tolower(uchar(s[i])));
			i += 1;
		}
	}
	luaL_pushresult(&b);
	return 1;
}

static int str_cp932_upper (lua_State *L) {
	size_t l;
	size_t i;
	luaL_Buffer b;
	const char *s = luaL_checklstring(L, 1, &l);
	luaL_buffinit(L, &b);
	for (i = 0; i < l;) {
		if (is_sjis_1(s[i + 0]) && is_sjis_2(s[i + 1])) {
			luaL_addchar(&b, s[i + 0]);
			luaL_addchar(&b, s[i + 1]);
			i += 2;
		} else {
			luaL_addchar(&b, toupper(uchar(s[i])));
			i += 1;
		}
	}
	luaL_pushresult(&b);
	return 1;
}

static int str_cp932_codepoint (lua_State *L) {
	size_t l;
	size_t i;
	int chr_cnt;
	int out_cnt;

	lua_Integer code;
	unsigned char *pcode;

	const char *s = luaL_checklstring(L, 1, &l);
	ptrdiff_t posi = posrelat(luaL_optinteger(L, 2, 1), l); // �Q�Ԃ߂̈���������΁B�Ȃ���΃f�t�H�l�Ƃ��ĂP
	ptrdiff_t pose = posrelat(luaL_optinteger(L, 3, posi), l); // �R�Ԗڂ̈���������΁A�Ȃ���΃f�t�H�l�Ƃ��ĂQ�Ԃ߂̈����Ɠ��l


	if (posi <= 0) posi = 1;
	if (posi > pose) return 0;  /* empty interval; return no values */

	pcode = (unsigned char *)&code;
	chr_cnt = 0; // �������Ƃ��ẴJ�E���g
	out_cnt = 0; // ���ۃX�^�b�N�Ƀv�b�V�������J�E���g
	for (i = 0; i < l && chr_cnt <= pose ;) {
		code = 0;
		chr_cnt++;
		if (is_sjis_1(s[i + 0]) && is_sjis_2(s[i + 1])) {

			// �J�n�`�I����index�̊Ԃ����o�͂���
			if ( posi <= chr_cnt && chr_cnt <= pose ) {
				// �����Ɛ��l�ł́A�o�C�g�P�ʂŒl���Ђ�����Ԃ�(�����ナ�g���G���f�B�A���ƃr�b�O�G���f�B�A��)
				pcode[0] = s[i+1];
				pcode[1] = s[i+0];
				lua_pushinteger(L, code);
				out_cnt++;
			}
			i += 2;

		} else {

			// �J�n�`�I����index�̊Ԃ����o�͂���
			if ( posi <= chr_cnt && chr_cnt <= pose ) {
				lua_pushinteger(L, s[i]);
				out_cnt++;
			}
			i++;

		}
	}
	return out_cnt;
}

// cp932�̂��߂ɓ��ʂɒǉ�
static int str_cp932_iter_aux (lua_State *L) {

	size_t l;

	lua_Integer code;
	unsigned char *pcode;

	const char *s = luaL_checklstring(L, 1, &l);
	lua_Integer i = lua_tointeger(L, 2) - 1; // iterator���痬��Ă��Ă���ԍ�

	pcode = (unsigned char *)&code;

	if (i < 0) { /* first iteration? */
		i = 0;  /* start from here */
	} else if (i < (lua_Integer)l ) {
		if ( is_lead_byte(s[i]) ) i += 2;  /* �}���`�o�C�g�̂悤�Ȃ�2�X�L�b�v */
		else i++;    /* skip current byte */
	}
	
	if ( i >= (lua_Integer)l ) {
		return 0;
	}

	code = 0;
	if (is_lead_byte(s[i + 0])) {

		// �����Ɛ��l�ł́A�o�C�g�P�ʂŒl���Ђ�����Ԃ�(�����ナ�g���G���f�B�A���ƃr�b�O�G���f�B�A��)
		pcode[0] = s[i+1];
		pcode[1] = s[i+0];
		lua_pushinteger(L, i+1);
		lua_pushinteger(L, code);
		return 2;

	} else {
		lua_pushinteger(L, i+1);
		lua_pushinteger(L, uchar(s[i]));
		return 2;
	}
	return 0;
}

// cp932�̂��߂ɓ��ʂɒǉ�
static int str_cp932_iter_codes (lua_State *L) {
  luaL_checkstring(L, 1);
  lua_pushcfunction(L, str_cp932_iter_aux);
  lua_pushvalue(L, 1);
  lua_pushinteger(L, 0);
  return 3;
}

static int str_cp932_char(lua_State *L) {
	int n = lua_gettop(L);  /* number of arguments */
	int i;
	int l = 0; // �S�̂̒���
	luaL_Buffer b;
	luaL_buffinit(L, &b);

	for (i = 1; i <= n; i++) {
		lua_Integer c = luaL_checkinteger(L, i);
		if (0 <= c && c <= 0xFF) {
			luaL_addchar(&b, uchar(c));
		}
		else if (0xFF < c && c <= 0xFFFF) {
			unsigned char *pcode = (char *)&c;
			luaL_addchar(&b, uchar(pcode[1]));
			luaL_addchar(&b, uchar(pcode[0]));
		}
	}

	luaL_pushresult(&b);
	return 1;
}



static int str_encode_guess (lua_State *L) {
	size_t len;
	const char *src = luaL_checklstring(L, 1, &len);

    enum NMH_CHAR_CODE n = nmh_code_detect(src, strlen(src));
	const char *szname = NMH_CHAR_CODE_NAME_LIST[n];
	lua_pushstring(L, szname);

	return 1;
}

// cp932��utf8 20150511 ���ʂɑ�������
static BOOL ConvSJistoUtf8( const BYTE* pSource, BYTE* pDist, int* pSize )
{
   int nSize;
   BYTE* buffUtf16;
   int nSizeUtf8;
   BYTE* buffUtf8;

   *pSize = 0;
   //ShiftJIS����UTF-16�֕ϊ�
   nSize = MultiByteToWideChar( CP_ACP, 0, (LPCSTR)pSource, -1, NULL, 0 );
 
   buffUtf16 = malloc( nSize * 2 + 2 );
   MultiByteToWideChar( CP_ACP, 0, (LPCSTR)pSource, -1, (LPWSTR)buffUtf16, nSize );
 
   //UTF-16����Shift-JIS�֕ϊ�
   nSizeUtf8 = WideCharToMultiByte( CP_UTF8, 0, (LPCWSTR)buffUtf16, -1, NULL, 0, NULL, NULL );
   if( !pDist ){
       *pSize = nSizeUtf8;
       free(buffUtf16);
       return TRUE;
   }
 
   buffUtf8 = malloc(nSizeUtf8 * 2);
   ZeroMemory( buffUtf8, nSizeUtf8 * 2 );
   WideCharToMultiByte( CP_UTF8, 0, (LPCWSTR)buffUtf16, -1, (LPSTR)buffUtf8, nSizeUtf8, NULL, NULL );
 
   *pSize = lstrlen( (char*)buffUtf8 );
   memcpy( pDist, buffUtf8, *pSize );
 
   free(buffUtf16);
   free(buffUtf8);
 
   return TRUE;
}

static int str_cp932_to_utf8 (lua_State *L) {
	size_t l;
	luaL_Buffer b;
	BYTE* pDist;
	int nSize;

	const char *s = luaL_checklstring(L, 1, &l);
	const char *ename = luaL_checklstring(L, 2, &l);

	// ��Q������"utf8"�Ȃ�cp932��utf8
	if ( strcmp((const char *)_strlwr((char *)ename), "utf8") == 0 ) {
		luaL_buffinit(L, &b);

		// ��������cp932��utf8�ւ̕ϊ�
		nSize = 0;
		// �P��ϊ���̃T�C�Y�𓾂邾��
		ConvSJistoUtf8( s, NULL, &nSize );
		// �T�C�Y�ɂ��������Ċm��
		pDist = malloc(nSize + 1);
		// 0�őS���N���A���āc
		ZeroMemory( pDist, nSize + 1 );
		// ���x��pDist�ɕϊ���̕�������i�[
		ConvSJistoUtf8( s, pDist, &nSize );

		// ����𕶎���Ƃ��ăv�b�V��
		luaL_addstring(&b, pDist);
		luaL_pushresult(&b);

		// ���
		free(pDist);

	// ����ȊO�͌��̂܂�
	} else {
		luaL_buffinit(L, &b);
		// ���̂��̂����̂܂�
		luaL_addstring(&b, s);
		luaL_pushresult(&b);
	}

	return 1;
}




static const luaL_Reg cp932lib[] = {
	{"char", str_cp932_char},
	{"codepoint", str_cp932_codepoint},
	{"len", str_cp932_len},
	{"lower", str_cp932_lower},
	{"upper", str_cp932_upper},
	{"reverse", str_cp932_reverse},
	{"sub", str_cp932_sub},
	{"codes", str_cp932_iter_codes},
	{"offset", str_cp932_offset},
	{"encode", str_cp932_to_utf8},

	{NULL, NULL}
};


/*
** Open cp932 library
*/
LUALIB_API int luaopen_cp932 (lua_State *L) {
	luaL_register(L, LUA_CP932LIBNAME, cp932lib);
	return 1;
}






// struct���C�u�����̂��̂��A���̂܂�string���C�u�����Ƃ���lua5.3�ō̗p���ꂽ�悤���B
extern int lstruct_b_pack (lua_State *L);
extern int lstruct_b_unpack (lua_State *L);
extern int lstruct_b_size (lua_State *L);


// string�̊g��lib
static const luaL_Reg string_ex_lib[] = {
	// struct���C�u��������̗��p
	{"pack", lstruct_b_pack},
	{"unpack", lstruct_b_unpack},
	{"packsize", lstruct_b_size},

	{"getencoding", str_encode_guess},
	{NULL, NULL}
};


/*
** String�ւ̒ǉ� library
*/
LUALIB_API int luaopen_string_ex (lua_State *L) {
	luaL_register(L, LUA_STRLIBNAME, string_ex_lib);
	return 1;
}
