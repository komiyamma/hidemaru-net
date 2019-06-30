#pragma once

#ifdef _DEBUG

#define    OutputDebugStream   _TheOutputDebugStream
extern void _TheOutputDebugStream(const char *format, ...);

#else

#define    OutputDebugStream   __noop

#endif

