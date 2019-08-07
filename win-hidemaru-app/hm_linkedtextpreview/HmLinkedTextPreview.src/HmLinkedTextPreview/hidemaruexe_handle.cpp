/*
 * Copyright(C) 2017 Akitsugu Komiyama
 * under the MIT License
 */ 

#include <windows.h>

#include "hidemaruexe_handle.h"
#include "hidemaruexe_export.h"


HidemaruWindowHandleSearcher::HidemaruWindowHandleSearcher(wstring strClassName) {
	this->strHidemaruClassName = strClassName;
	this->hCurWndHidemaru = NULL;
}

HWND HidemaruWindowHandleSearcher::GetCurWndHidemaru() {
	// �܂����ʂ̃^�u���[�h�̏G��
	if (!hCurWndHidemaru) {
		TabOnSearchCurWndHidemaru(GetDesktopWindow());
	}
	// ���Ƀ^�u�ł͂Ȃ����[�h�̏G��
	if (!hCurWndHidemaru) {
		NoTabSearchCurWndHidemaru(GetDesktopWindow());
	}

	// �ȏ�̂Q�̏��ԂŌ������Ă����K�v������
	return this->hCurWndHidemaru;
}



bool HidemaruWindowHandleSearcher::IsWndHidemaru32ClassType(HWND hWnd) {
	if (!hWnd) {
		return false;
	}

	wchar_t strTargetClassName[MAX_CLASS_NAME];
	::GetClassName(hWnd, strTargetClassName, _countof(strTargetClassName));

	if (strHidemaruClassName == strTargetClassName) {
		return true;
	}
	return false;
}

// ������肾���A�G�ۑ��̃A�v���̎q���ɂȂ��Ă�ȂǁA�����A�P�[�X�ł������o����B
void HidemaruWindowHandleSearcher::TabOnSearchCurWndHidemaru(HWND hWnd)
{
	if (hCurWndHidemaru) { return; }

	HWND hWndParent = ::GetParent(hWnd);

	// �e�����邱�Ƃ�����
	if (hWndParent) {
		// �����̃v���Z�XID�ƁA�T�[�`�Ώۂ̃v���Z�XID
		DWORD pID1 = GetCurrentProcessId();
		DWORD pID2 = 0;
		GetWindowThreadProcessId(hWnd, &pID2);
		// �����Ȃ�傫����₾
		if (pID1 == pID2) {

			// �������g�̐e���w��̃N���X���Ȃ�A���S�ɓ��肵���B
			if (IsWndHidemaru32ClassType(hWndParent))
			{
				hCurWndHidemaru = hWnd;
			}
		}
	}

	// �q�N���X���Ȃ߂Ă����B�q�N���X�͂����܂ł��uHidemaru32Class�n�v�B
	// �X�g�A�ł͂�����ƈႤ�̂ŃC���X�^���X�ϐ��ɂȂ��Ă���B
	const wchar_t *pszClassName = strHidemaruClassName.c_str();
	HWND hWndChild = FindWindowEx(hWnd, NULL, pszClassName, NULL);
	while (hWndChild != NULL)
	{
		TabOnSearchCurWndHidemaru(hWndChild);
		if (hCurWndHidemaru) { break; }

		hWndChild = FindWindowEx(hWnd, hWndChild, pszClassName, NULL);
	}
}

// �����B�ʏ�Ȃ炱�̃A���S���Y���ŏ\��
void HidemaruWindowHandleSearcher::NoTabSearchCurWndHidemaru(HWND hWnd)
{
	if (hCurWndHidemaru) { return; }

	HWND hWndParent = ::GetParent(hWnd);

	if (IsWndHidemaru32ClassType(hWnd)) {
		// �����̃v���Z�XID�ƁA�T�[�`�Ώۂ̃v���Z�XID
		DWORD pID1 = GetCurrentProcessId();
		DWORD pID2 = 0;
		GetWindowThreadProcessId(hWnd, &pID2);
		// �����Ȃ�傫����₾
		if (pID1 == pID2) {

			hCurWndHidemaru = hWnd;
		}
	}

	// �q�N���X���Ȃ߂Ă����B�q�N���X�͂����܂ł��uHidemaru32Class�n�v�B
	// �X�g�A�ł͂�����ƈႤ�̂ŃC���X�^���X�ϐ��ɂȂ��Ă���B
	const wchar_t *pszClassName = strHidemaruClassName.c_str();
	HWND hWndChild = FindWindowEx(hWnd, NULL, pszClassName, NULL);
	while (hWndChild != NULL)
	{
		NoTabSearchCurWndHidemaru(hWndChild);
		if (hCurWndHidemaru) { break; }

		hWndChild = FindWindowEx(hWnd, hWndChild, pszClassName, NULL);
	}
}

HWND GetCurWndHidemaru(HWND hCurWnd) {
	// ���󂪋@�\���Ă���Ȃ炻�̂܂�
	if (IsWindow(hCurWnd)) {
		return hCurWnd;
	}

	// ���X�͂����Ă��Ȃ�������A�������́A�^�u���[�h�̐؂�ւ����ŋ@�\���Ȃ��Ȃ���

	HWND hWnd = NULL;
	if (!hWnd) {
		HidemaruWindowHandleSearcher s1(L"Hidemaru32Class");
		hWnd = s1.GetCurWndHidemaru();
	}

	if (!hWnd) {
		// �X�g�A�A�v����
		HidemaruWindowHandleSearcher s2(L"Hidemaru32Class_Appx");
		hWnd = s2.GetCurWndHidemaru();
	}

	return hWnd;
}
