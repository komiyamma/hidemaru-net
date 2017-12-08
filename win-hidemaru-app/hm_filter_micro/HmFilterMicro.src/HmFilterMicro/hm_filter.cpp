/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the MIT License
 */

#define _CRT_SECURE_NO_WARNINGS
#include "hm_filter.h"

using namespace std;

HGLOBAL utf16_to_hm_filter_global(const wstring& wstr_text) {
	// wstring�^�Ȃ̂ŁA�K���Q�{�B
	size_t cb = (wstr_text.length() + 1) * 2;

	HGLOBAL	hmemDst = GlobalAlloc(GMEM_MOVEABLE, cb);
	if (hmemDst == nullptr) {
		MessageBox(NULL, L"���������\���m�ۂł��Ȃ�", L"���������\���m�ۂł��Ȃ�", NULL);
		return nullptr;
	}

	// ���b�N
	wchar_t* pwchDest = (wchar_t*)GlobalLock(hmemDst);
	// �R�s�y
	wcscpy(pwchDest, wstr_text.c_str());
	// �A�����b�N
	GlobalUnlock(pwchDest);
	return pwchDest;
}


//---------------------------------------------------------------------------------------------------
// ���ʂ̃o�[�W�������l �� �G�ەϊ����W���[���^�C�v�̃o�[�W�����ɕϊ�����
//---------------------------------------------------------------------------------------------------
int double_ver_to_hm_filter_ver(const double ver) {
	// �}�C�i�[�o�[�W�����������o���āc
	double minor = ver - (int)ver;

	// ���W���[�o�[�W�����������o����
	int major = int(ver - minor);

	// �G�ۂ��~����`�ւƕϊ�
	return (major << 16) + int(100 * minor);
}
