/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the MIT License
 */

#pragma once

#ifdef _DEBUG

// デバッグモニターに出力(cp932を超える分は潰れる)
void OutputDebugStream(wstring wstr);
void OutputDebugStream(string str);

#else

#define OutputDebugStream __noop

#endif