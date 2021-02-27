/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#pragma once
#include <windows.h>
#include <string>

using namespace std;

extern HWND GetCurWndHidemaru(HWND hCurWnd);

// �X�g�A�A�v���ł��l�����Ē��^�ɂ��Ă���
class HidemaruWindowHandleSearcher {

public:
	HidemaruWindowHandleSearcher(wstring strClassName);

private:
	// ����̓X�^�e�B�b�N�ł͂Ȃ��B�f�X�N�g�b�v�łƃA�v���ł�����̂�
	wstring strHidemaruClassName;
	// �G�ۃn���h��
	HWND hCurWndHidemaru;

public:
	HWND GetCurWndHidemaru();


private:
	bool IsWndHidemaru32ClassType(HWND hWnd);
	// �G�ۂ��ϑ��I�Ȃ��ƂɂȂ��Ă��Ȃ��i�Ⴆ�΁A���̃A�v���̎q�E�B���h�E�Ƃ��ɋ�����parentWindows��P���Ȃ����Ă���ȂǂƂ��������ƂɂȂ��Ă��Ȃ�)
	void TabOnSearchCurWndHidemaru(HWND hWnd);
	void NoTabSearchCurWndHidemaru(HWND hWnd);

};
