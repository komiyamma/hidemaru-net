#pragma once

#include <windows.h>
#include <string>

using namespace std;

//-------------------------------------------------------------------------
// dll���g�̃n���h����t�X�p�X�̏��̕ێ�
//-------------------------------------------------------------------------
struct CSelfDllInfo {

	//-------------------------------------------------------------------------
	// �������g(hmPerl.dll)�̃��W���[���C���X�^���X�n���h��
	static HMODULE hModule;

	//-------------------------------------------------------------------------
	// �������g(hmPerl.dll)�̃t���p�X
	static wchar_t szSelfModuleFullPath[MAX_PATH];

	static wchar_t szSelfModuleDirPath[MAX_PATH];

	//-------------------------------------------------------------------------
	// ����dll���G�ۃ}�N������ǂ̂悤�Ȍ`��loaddll���ꂽ�̂��̏��B
	// ���̏�񂪂���΁Adll��������}�N���𔭍s���邱�Ƃ��o����B
	// -1   :loaddll���ő������ꂽ
	// 0    :�ǂ߂Ă��Ȃ��B(�ǂ߂ĂȂ�������dll���s����ĂȂ��̂ŁA����͂��蓾�Ȃ�)
	// 1�ȏ�:���̐��l�ŏG�ۃ}�N����ő�������Ă���B
	//-------------------------------------------------------------------------
private:
	static int iSelfBindedType;
public:
	static void InitializeHandle(HMODULE handle);

	static int GetBindDllType();
	static BOOL SetBindDllHandle();
	static wstring GetInvocantString();
	static wstring GetSelfModuleFullPath();
	static wstring GetSelfModuleDir();
};


