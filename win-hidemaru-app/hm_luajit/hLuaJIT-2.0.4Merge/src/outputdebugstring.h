#include <stdio.h>
#include <stdlib.h>


// コンソールインタプリタの場合
#ifdef LUA_CONSOLE_INTERPRETER

#define ODSfwrite	fwrite
#define ODSfputs    fputs
#define ODSputs		puts
#define ODSputc		putc
#define ODSfprintf	fprintf
#define ODSprintf   printf


// BGMmmの場合
#else

size_t ODSfwrite(const void *buf, size_t size, size_t n, FILE *fp);
int ODSfputs( const char *str , FILE *fp );
int ODSputs( const char *str );
int ODSputc( int c, FILE *stream );
int ODSfprintf( FILE *stream, char *format, ... );
int ODSprintf( char *format, ... );

#endif