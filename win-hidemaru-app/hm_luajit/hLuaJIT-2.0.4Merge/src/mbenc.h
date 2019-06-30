/*
** mbenc.h : lead-byte checking functions for multi-byte encoding
**
** multi-byte encoding support for Lua
** by ham
**   - OS/Architecture independent
**   - using ANSI-C only 
*/

#ifndef mbenc_h
#define mbenc_h

/* set one of these macros to 1 */
/* if all values are 0, lua just process as single-byte encoding */
/* EUC and UTF-8 is just ok with default single-byte mode */
#define LUA_USE_ENC_SHIFT_JIS 1
#define LUA_USE_ENC_ANSI 0

/* -- japanese shift jis -- */
#if LUA_USE_ENC_SHIFT_JIS
#define is_sjis_lead_byte(c) \
  ((0x81 <= ((unsigned char) (c)) && ((unsigned char) (c)) <= 0x9f) \
  || (0xe0 <= ((unsigned char) (c)) && ((unsigned char) (c)) <=0xfc))
#define is_lead_byte(c) is_sjis_lead_byte(c)
#endif


#define is_sjis_1(c) \
 (((0x81 <= ((c) & 0xff)) && (((c) & 0xff) <= 0x9f)) || \
  ((0xe0 <= ((c) & 0xff)) && (((c) & 0xff) <= 0xef)))

#define is_sjis_2(c) \
 (((0x40 <= ((c) & 0xff)) && (((c) & 0xff) <= 0x7e)) || \
  ((0x80 <= ((c) & 0xff)) && (((c) & 0xff) <= 0xfc)))

#define make_wc(s) \
 ((is_sjis_1((s)[0]) && is_sjis_2((s)[1])) ? \
  ((((s)[0] & 0xff) << 8) | ((s)[1] & 0xff)) : \
  (((s)[0]) & 0xff))

#define make_wclm(s, l) \
 (((l >= 2) && is_sjis_1((s)[0]) && is_sjis_2((s)[1])) ? \
  ((((s)[0] & 0xff) << 8) | ((s)[1] & 0xff)) : \
  (((s)[0]) & 0xff))

#define mb_len(p) ((is_sjis_1((p)[0]) && is_sjis_2((p)[1])) ? 2 : 1)

#define mb_next(p) (&((p)[mb_len(p)]))

#define mb_prev(ep) \
  ((is_sjis_1((ep)[-2]) && is_sjis_2((ep)[-1])) ? \
   (&((ep)[-2])) : (&((ep)[-1])))

/* -- ANSI method --
   if you use this method, put
     setlocale(LC_ALL, ""); 
   on your main routine before executing lua functions.
*/
#if LUA_USE_ENC_ANSI
#include <ctype.h>
#define is_lead_byte(c) isleadbyte(c)
#endif

/* if no multi-byte encoding selected, just return 0(=false) */
#ifndef is_lead_byte
#define is_lead_byte(c) (0)
#endif

#endif
