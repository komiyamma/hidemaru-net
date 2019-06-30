#pragma once
#include <windows.h>
#include <string>

using namespace std;
using namespace System;
extern HWND GetCurWndHidemaru(HWND hCurWnd);

// �X�g�A�A�v���ł��l�����Ē��^�ɂ��Ă���
public ref class HidemaruWindowHandleSearcher {

public:
	HidemaruWindowHandleSearcher(String^ strClassName);

private:
	// ����̓X�^�e�B�b�N�ł͂Ȃ��B�f�X�N�g�b�v�łƃA�v���ł�����̂�
	String^ strHidemaruClassName;
	// �G�ۃn���h��
	HWND hCurWndHidemaru;

public:
	HWND SearchCurWndHidemaru();

public:
	// �O�Ƃ̐ړ_
	static IntPtr GetCurWndHidemaru(IntPtr curWnd);


private:
	bool IsWndHidemaru32ClassType(HWND hWnd);
	// �G�ۂ��ϑ��I�Ȃ��ƂɂȂ��Ă��Ȃ��i�Ⴆ�΁A���̃A�v���̎q�E�B���h�E�Ƃ��ɋ�����parentWindows��P���Ȃ����Ă���ȂǂƂ��������ƂɂȂ��Ă��Ȃ�)
	void TabOnSearchCurWndHidemaru(HWND hWnd);
	void NoTabSearchCurWndHidemaru(HWND hWnd);

}; 
