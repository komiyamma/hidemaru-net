/*
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#pragma once


/*--------------------------------------------------------------------------------------------------------------------------
int WINAPI Hidemaru_GetDllFuncCalledType(int n);
�@ n�� - 1���w�肷��ƁAloaddll�̂������Ԃ��܂��B1�ȏ��loaddll�֐��̕Ԃ�l�Ɠ����ł��B
�@ - 1�̏ꍇ��loaddll���œǂݍ��܂ꂽDLL�ł��B

  �A n��0���w�肷��ƁAdllfunc / dllfuncw / dllfuncstr / dllfuncstrw�̂ǂ�ŌĂ΂ꂽ���ɂ���āAreturn�ŕԂ��ׂ��l�������܂��B
  �@ �ȉ��̒l�̂����ꂩ���Ԃ�܂��B
	--------------------------------------------------------------------------------------------------------------------------*/
#define DLLFUNCRETURN_NOTCALL   0       //�Ă΂�Ă��Ȃ�
#define DLLFUNCRETURN_INT       0x0001  //int��Ԃ��ׂ�(dllfunc�܂���dllfuncw)
#define DLLFUNCRETURN_CHAR_PTR  0x0002  //char*��Ԃ��ׂ�(dllfuncstr)
#define DLLFUNCRETURN_WCHAR_PTR 0x0003  //WCHAR*��Ԃ��ׂ�(dllfuncstrw)
#define DLLFUNCRETURN_DOUBLE    0x0004  //double��Ԃ��ׂ�(dllfunc�܂���dllfuncw�ŕ��������_�����L��)

/*--------------------------------------------------------------------------------------------------------------------------
�B n��1���琔�������Ԃ̒l���w�肷��ƁA1���琔�����p�����[�^�̎�ނ����l / ������ / Unicode������ / ���������_������Ԃ��܂��B
�@ �p�����[�^�̍ő��64�ł��B
  �ȉ��̒l�̂����ꂩ���Ԃ�܂��B
  --------------------------------------------------------------------------------------------------------------------------*/
#define DLLFUNCPARAM_NOPARAM    0       //�ȍ~�̃p�����[�^�Ȃ�
#define DLLFUNCPARAM_INT        0x0100  //int�̃p�����[�^
#define DLLFUNCPARAM_CHAR_PTR   0x0200  //char*�̃p�����[�^
#define DLLFUNCPARAM_WCHAR_PTR  0x0300  //WCHAR*�̃p�����[�^
#define DLLFUNCPARAM_DOUBLE     0x0400  //double�̃p�����[�^

/*--------------------------------------------------------------------------------------------------------------------------
dllfunc�����Ă�ł���ԁA�Ō��dllfunc�����Ă񂾓��e�̂ݎ擾�ł��܂��B
�Ⴆ��dllfunc�֐�����Hidemaru_EvalMacro�ł����dllfunc���Ăяo���ꍇ�A�擾�ł�����͈̂�ԐV�����Ă񂾂��݂̂̂ł��B
--------------------------------------------------------------------------------------------------------------------------*/

// hidemaru.exe����o�Ă���֐����擾�B����C#�w�Ŏ擾���邪�A���̊֐���C++�C���^�[�t�F�C�X�p�Ȃ̂ŁA��O�I�ɂ����Ŏ擾
using PFNGetDllFuncCalledType = int(WINAPI *)(int n);

extern PFNGetDllFuncCalledType Hidemaru_GetDllFuncCalledType;

extern bool InitHidemaruFunctions();
