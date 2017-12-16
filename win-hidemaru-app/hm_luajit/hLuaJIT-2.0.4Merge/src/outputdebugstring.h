/* 
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the MIT License
 */

#include <stdio.h>
#include <stdlib.h>


// �R���\�[���C���^�v���^�̏ꍇ
#ifdef LUA_CONSOLE_INTERPRETER

#define ODSfwrite	fwrite
#define ODSfputs    fputs
#define ODSputs		puts
#define ODSputc		putc
#define ODSfprintf	fprintf
#define ODSprintf   printf


// BGMmm�̏ꍇ
#else

size_t ODSfwrite(const void *buf, size_t size, size_t n, FILE *fp);
int ODSfputs( const char *str , FILE *fp );
int ODSputs( const char *str );
int ODSputc( int c, FILE *stream );
int ODSfprintf( FILE *stream, char *format, ... );
int ODSprintf( char *format, ... );

#endif