/*
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the MIT License
 */

#pragma once

// szName���t���p�X�Ɋ܂܂��v���Z�X�h�c���擾�B�ŏ��Ɍ���������B
// �����m�����ɂ�⌇����̂ŁA�\�Ȍ���N���X��(TARGET_CLASS_WND)�őΏ����������悢�B
int getTargetProcessID ( char *szNamePattern );

// �w��̃t�@�C�����̃v���Z�X���������s���ꂢ�邩�𓾂�
int getTargetProcessCount(char* process);