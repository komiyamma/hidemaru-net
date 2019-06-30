/*
 * Copyright(C) 2017 Akitsugu Komiyama
 * under the MIT License
 */ 

#pragma once

#include <windows.h>
#include <string>

using namespace std;

class CHidemaruExeExport {
public:
	CHidemaruExeExport();
private:
	static HMODULE hHideExeHandle;
	static TCHAR szHidemaruFullPath[MAX_PATH];

private:
	//========================================================================
	// �G�ۖ{�̂̃o�[�W�����B
	//-------------------------------------------------------------------------
	// 8.66��3�Ȃ�A�u866.03�v�݂����Ȋ����ɂȂ�B
	//========================================================================
	static double hm_version;
	static double QueryFileVersion(TCHAR* path);

protected:
	//========================================================================
	/// �G�ۖ{�̂���Export����Ă��邢�����̊֐��Q
	//========================================================================

	//-------------------------------------------------------------------------
	// ���ݕҏW���̑S�Ẵe�L�X�g�𓾂�
	using PFNGetTotalTextUnicode = HGLOBAL(WINAPI *)(void);
	static PFNGetTotalTextUnicode Hidemaru_GetTotalTextUnicode;

	using PFNGetCurrentWindowHandle = HWND(WINAPI *)(void);
	static PFNGetCurrentWindowHandle Hidemaru_GetCurrentWindowHandle;

public:


	//========================================================================
	/// ���b�v�֐��S
	//========================================================================

	//-------------------------------------------------------------------------
	// Hidemaru_GetTotalTextUnicode�֐��̃��b�v��������
	//-------------------------------------------------------------------------
	static wstring GetTotalText();

private:
	static HWND hCurWndHidemaru;
public:
	static HWND GetCurWndHidemaru();
};


