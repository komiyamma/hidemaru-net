/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the MIT License
 */

#pragma once

#include <windows.h>
#include <string>

#include "convert_string.h"
#include "output_debugstream.h"


using namespace std;

#define MACRO_DLL extern "C" __declspec(dllexport) 

//---------------------------------------------------------------------------------------------------
// hwnHidemaru   : �G�ۂ̃E�B���h�E�n���h��
// pwszIn        : wchar_t* �^�� �ϊ��ΏۂƂȂ��Ă��镶���񂪑S���n���Ă���B
// pszParamBuffer: �u�G�ۃ}�N����filter�֐��v�œn�����ƂɂȂ��R�����̕�����B
//                ����͎����Ŏ��R�Ɏ����Ō��߂�A�p�����[�^�݂����Ȃ��́B
//                �ʏ�́u0�v�Ɓu1�v�̕��тƂ��邱�Ƃ��������A�������g��Parse����̂ŉ��ł��ǂ����낤�B
// 
//                �Ⴆ�΁A�G�ۂ́uSort�ϊ����W���[���v�̏ꍇ�́A
//                fReverse     = pszParamBuffer[0] == '1' or '0';
//                fIgnoreSpace = pszParamBuffer[1] == '1' or '0';
//                fIgnoreCase  = pszParamBuffer[2] == '1' or '0';
//                fRealNumber  = pszParamBuffer[3] == '1' or '0';
//
//                ���A�p�����[�^��ǂݎ�邾���ł͂Ȃ��A��������ł������Ƃ��\�B
//                ��������ł��������̂́A�u�L�[����̋L�^�v������ƁA�G�ۃG�f�B�^�����̃p�����[�^���o����B
//                �L�[����̍Đ�������ƁA�G�ۃG�f�B�^�Ŋo���Ă����p�����[�^��t���ĕϊ����W���[�����Ăяo���悤�ɂȂ�A
//                �_�C�A���O���o�����ɑO��Ɠ����I���Ŏ��s���o����悤�ɂ��ݏグ�邱�Ƃ��\�ɂȂ��Ă��܂��B
//
// cbParamBuffer : pszParam�̏G�ۖ{�̑��̃o�b�t�@�[�T�C�Y�B260�B
//---------------------------------------------------------------------------------------------------
using HIDEMARUFILTERFUNC = HGLOBAL(_cdecl *)( HWND hwndHidemaru, const wchar_t* pwszIn, char* pszParamBuffer, int cbParamBuffer);


struct HIDEMARUFILTERINFO {
	UINT                cbStructSize;
	const char*         pszExportName;
	const char*         pszNameJapan;
	const char*         pszNameUs;
	BYTE                chAccel;        // �A�N�Z�����[�^�L�[
	BYTE                fMustLineUnit;  // �s�P�ʂłȂ��Ǝ��s�ł��Ȃ��t���O
	BYTE                abReserve[2];
};


#define WM_HIDEMARUINFO (WM_USER + 181)

#define HIDEMARUINFO_GETTABWIDTH	0
#define HIDEMARUINFO_GETSTARTCOLUMN 1
#define HIDEMARUINFO_GETSPACETAB	2



// utf16��global�ցB����͉���������ł���Ă��Ȃ��̂ŏG�ەϊ����W���[����p
HGLOBAL utf16_to_hm_filter_global(const wstring& wstr_text);

// ���ʂ̏������̃o�[�W�������G�ۂ��v������`�̃o�[�W�������l��
int double_ver_to_hm_filter_ver(const double ver);
