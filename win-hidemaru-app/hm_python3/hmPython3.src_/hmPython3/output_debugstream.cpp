#include <windows.h>
#include <tchar.h>
#include <sstream>

#include "convert_string.h"

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

