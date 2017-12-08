/*
 Copyright (c) 2016-2017 Akitsugu Komiyama
 under the Apache License Version 2.0
*/

#pragma once
#include <tchar.h>

#include <windows.h>
#include <regex>
#include <string>

#include "perlez.h"

using namespace std;


class CPerlEzEngine {

private:
	//========================================================================
	// PerlEz.dll��dll���̂̃n���h��
	//========================================================================
	static HMODULE hPerlEzDll;
	// �Z�b�e�B���O�p��XML��p�ӂ��Ă���ꍇ�ɁAPerlEz�̃t���p�X���w�肷�邱�Ƃ��o����B
	static wstring GetPerlEzPath();
public:
	//-------------------------------------------------------------------------
	// PerlEz���Ǘ�����Perl�X�N���v�g�G���W���̃n���h��
	static PERLEZHANDLE engine;

private:
	//========================================================================
	/// PerlEz����Export����Ă��邢�����̊֐��Q�̌^�ێ�
	//========================================================================

	//-------------------------------------------------------------------------
	// �G���W���̐���
	using TPerlEzCreate = decltype(::PerlEzCreate)*;

	//-------------------------------------------------------------------------
	// �G���W���̔j��
	using TPerlEzDelete = decltype(::PerlEzDelete)*;

	//-------------------------------------------------------------------------
	// Perl�O���[�o���X�R�[�v��ɂ���֐��������t�ŌĂяo��
	using TPerlEzCall = decltype(::PerlEzCall)*;

	//-------------------------------------------------------------------------
	// Perl�O���[�o���X�R�[�v��ɑ΂��āAPerl�X�N���v�g�����̏�Ŏ��s���A�]�����ʂ𓾂�
	using TPerlEzEvalString = decltype(::PerlEzEvalString)*;

	//-------------------------------------------------------------------------
	// �O���[�o���ϐ��ɑ΂��āA�u�擾�v�Ɓu����v�̓x�ɁAC����̃t�b�N�֐����Ăяo�����悤��
	// ���ʂȃO���[�o���ϐ�����肾���B
	using TPerlEzSetMagicScalarName = decltype(::PerlEzSetMagicScalarName)*;

	//-------------------------------------------------------------------------
	// ��L���ׂĂ̓��ʂȃO���[�o���ϐ��ŁA���p����t�b�N�p�֐���ݒ肷��B
	// �u�ǂݎ�莞�p�v�Ɓu�������ݎ��p�v�̂Q���
	// ��Q�����ɃO���[�o���ϐ��̕ϐ������̂��n���Ă���̂ŁA����ŏ�����U�蕪����B
	using TPerlEzSetMagicScalarFunctions = decltype(::PerlEzSetMagicScalarFunctions)*;

public:
	//========================================================================
	/// PerlEz����Export����Ă��邢�����̊֐��Q
	//========================================================================
	TPerlEzCreate     PerlEzCreate; // ��P������""�ɂ���΁A�t�@�C����ǂ܂Ȃ��Ă��ǂ��B
	TPerlEzDelete     PerlEzDelete;

	TPerlEzCall       PerlEzCall;
	TPerlEzEvalString PerlEzEvalString;

	TPerlEzSetMagicScalarName      PerlEzSetMagicScalarName;
	TPerlEzSetMagicScalarFunctions PerlEzSetMagicScalarFunctions;

public:
	CPerlEzEngine();
	~CPerlEzEngine();

private:
	//========================================================================
	// IronPython/DLR�̂̂悤�ɁA�����x�������u�N�����x�X�g�Ǝv���ł��낤�\�L�v�����݂���̂Ƃ͈قȂ�
	// Perl�̏ꍇ�́A�D�݂ŋL�q���������킩��Ă��܂����߁Ahm->Macro->***��hm->Edit->***�Ƃ�����
	// �\�L��hmPerl.pm�Ƃ����`�ŊO�ւƏo���āA���R�ɃJ�X�^���o����悤�ɂ����B
	//========================================================================

	//========================================================================
	// hmPerl.pm�̃t���p�X
	TCHAR szPerlCreateFileFullPath[MAX_PATH];


public:
	//-------------------------------------------------------------------------
	// �G���W���͗L��(�ǂݍ��ݍ�)���ǂ����B
	static BOOL IsValid();

	// �Ώۂ̕�����̓p�[��Eval�G���[�ۂ��B
	BOOL IsReturnBufferSeemsEvalError(char *utf8_buffer);

	struct TEvalErrorInfo {
		bool HasError;		// �G���[�����������ǂ����B
		int lineno;			// �G���[�s
		int column;			// �G���[�ʒu
		wstring errorlinecode;	// �G���[���o���s�̕�����
	};

	// �G���[�̈ʒu�Ə��𕪐�
	TEvalErrorInfo GetEvalErrorInfo(wstring strAllSrcCode, char *utf8_buffer);
};


// �֐��̒ǉ�����

/*
char *pfilename = "C:/test.pl"

-------------------------------------------------------------------------------------
int ret = PerlEzCreateOpt(pfilename, "", "market C:\\usr\\perl\\bin\\corpus.txt")
-------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------
int ret = PerlEzCreate("", ""); �̂悤�ɂ���΁A�t�@�C���͓ǂ܂Ȃ��Ă��ǂ��B
-------------------------------------------------------------------------------------

*/