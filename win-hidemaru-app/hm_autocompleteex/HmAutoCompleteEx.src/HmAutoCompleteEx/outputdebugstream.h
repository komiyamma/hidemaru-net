/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#pragma once

#ifdef _DEBUG

#define    OutputDebugStream   _TheOutputDebugStream
extern void _TheOutputDebugStream(const TCHAR *format, ...);

#else

#define    OutputDebugStream   __noop

#endif

