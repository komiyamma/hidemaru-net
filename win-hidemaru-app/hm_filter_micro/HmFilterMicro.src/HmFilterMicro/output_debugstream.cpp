/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the MIT License
 */

#include <windows.h>
#include <sstream>

#include "convert_string.h"


#ifdef _DEBUG

void OutputDebugStream(wstring wstr) {
	string cp932_data = utf16_to_cp932(wstr);

	// ���������͉��s�ŋ�؂��ďo���Ă����B
	// �f�o�b�O���j�^�[�̐������l����ƁA���̕����Ȃɂ��Ɨǂ��B
	std::stringstream is(cp932_data);
	string t;
	while (getline(is, t))
	{
		t.erase(t.find_last_not_of("\r") + 1);
		OutputDebugStringA(t.data());
		OutputDebugStringA("\n");
	}
}

void OutputDebugStream(string str) {
	OutputDebugStream(cp932_to_utf16(str));
}

#endif