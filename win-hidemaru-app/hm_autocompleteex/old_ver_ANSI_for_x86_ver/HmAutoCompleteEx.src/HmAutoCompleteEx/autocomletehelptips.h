#pragma once

#include <windows.h>
#include <string>

using namespace std;

using PFNCREATE = int(*)(HWND hWnd, LPCTSTR szFileName);
using PFNDESTROY = int(*)(HWND hWnd);
using PFNSELECT = int(*)(HWND hWnd, int iListBoxSelectedIndex, LPCTSTR szListBoxSelectedItem, int iItemHeight);
using PFNKEYDOWN = int(*)(HWND hWnd, WPARAM wParam);

#define AUTOCOMPLETELIST_MAX_COUNT 1000
struct AUTOCOMPLETELIST {
	LPTSTR string_ptr_list[AUTOCOMPLETELIST_MAX_COUNT];
	size_t list_size;
};
using PFNADDLIST = AUTOCOMPLETELIST* (*) (HWND hWnd, AUTOCOMPLETELIST *pAutoCompleteOriginalList);

namespace AutoCompleteHelpTip {
	// �����⊮���̃w���v�`�b�v�̏���
	int OnCreate(HWND hWnd, string filename);

	// ���͕⊮���ɁA�⊮���X�g�̑I����ύX�����B(�L�[�{�[�h�̏㉺��}�E�X�Ȃǂɂ����)
	int OnListBoxSelectedIndexChanged(HWND hWnd, int iListBoxSelectedIndex, string szListBoxSelectedItem, RECT rect, int iItemHeight);

	// �����⊮���̃w���v�`�b�v�̔j��
	int OnDestroy(HWND hWnd);

	// ���ʂɂ��̏u�ԂɁA���͕⊮�ɑ������ނ��߂̃��X�g�����肷��B
	vector<string> OnQueryListBoxCustomAdded(HWND hWnd, vector<string>& original_list, string filename);

	// �G�f�B�^���⎩���⊮���ŃL�[�������ꂽ��
	int OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
}

//----------------------------------------------------------------------
// DLL�֘A

struct BINDAUTOCOMPLETEDLL {
	string strRegexp;
	string strDllName;
	HMODULE hDllModule;
	PFNCREATE  cr;
	PFNDESTROY dr;
	PFNSELECT  sl;
	PFNKEYDOWN kd;
	PFNADDLIST ca;

	BINDAUTOCOMPLETEDLL() {
		hDllModule = NULL;
		cr = NULL;
		dr = NULL;
		sl = NULL;
		kd = NULL;
		ca = NULL;
	}
};

extern vector< BINDAUTOCOMPLETEDLL> mapBindAutoCompleteDll;

