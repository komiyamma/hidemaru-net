/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the MIT License
 */

#pragma once

#ifdef _DEBUG

// �f�o�b�O���j�^�[�ɏo��(cp932�𒴂��镪�ׂ͒��)
void OutputDebugStream(wstring wstr);
void OutputDebugStream(string str);

#else

#define OutputDebugStream __noop

#endif