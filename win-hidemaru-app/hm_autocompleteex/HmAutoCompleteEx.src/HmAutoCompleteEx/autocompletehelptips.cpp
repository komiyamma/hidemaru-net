/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <vector>
#include <locale>         // std::locale, std::tolower
#include <shlwapi.h>

#include "tstring.h"

using namespace std;

#include "outputdebugstream.h"
#include "autocomletehelptips.h"
#include "sethidemarufile.h"



namespace AutoCompleteHelpTip {

	BINDAUTOCOMPLETEDLL* GetBindingDll() {
		auto filename = GetCurrentFileName();
		for (auto&mo : mapBindAutoCompleteDll) {
			tregex re(mo.strRegexp, regex_constants::icase);
			tcmatch m;
			if (regex_search(filename, m, re)) {
				return &mo;
			}
		}

		return NULL;
	}

	/*----------------------------------------------------------------------------------------------------------------------
	* ���͕⊮�E�B���h�E���������ꂽ����ɌĂ΂��R�[���o�b�N�֐�
	*----------------------------------------------------------------------------------------------------------------------
	* hWnd     : ���X�g�{�b�N�X�E�B���h�E
	* filename : �t�@�C����
	* (���X�g�{�b�N�X�E�B���h�E��SendMessage�Ƃ������Ȃ����ƁB�v�Z������)
	*/
	int OnCreate(HWND hWnd, tstring filename) {
		if (mapBindAutoCompleteDll.size() == 0) {
			return FALSE;
		}
		BINDAUTOCOMPLETEDLL *m = GetBindingDll();
		if (!m) {
			return FALSE;
		}

		if (m->cr) {
			OutputDebugStream(_T("����cr�Ăяo��"));
			m->cr(hWnd, filename.data());
		}


		OutputDebugStream(_T("�����͕⊮�E�B���h�E�������ꂽ����\n"));
		return TRUE;
	}


	/*----------------------------------------------------------------------------------------------------------------------
	 * ���͕⊮�E�B���h�E�̍��ڑI�����ω������ꍇ�ɌĂ΂��R�[���o�b�N�֐�
	 *----------------------------------------------------------------------------------------------------------------------
	 * hWnd                   : ���X�g�{�b�N�X�E�B���h�E
	 * iListBoxSelectedIndex  : ���X�g�{�b�N�X�ŉ��Ԃ߂��I������Ă��邩
	 * strListBoxSelectedItem : ���e
	 * rect                   : �E�B���h�E�̋������S�_�̍��W
	 * (���X�g�{�b�N�X�E�B���h�E��SendMessage�Ƃ������Ȃ����ƁB�v�Z������)
	 */
	int OnListBoxSelectedIndexChanged(HWND hWnd, int iListBoxSelectedIndex, tstring strListBoxSelectedItem, RECT rect, int iItemHeight) {
		if (mapBindAutoCompleteDll.size() == 0) {
			return FALSE;
		}

		BINDAUTOCOMPLETEDLL*m = GetBindingDll();
		if (!m) {
			return FALSE;
		}

		if (m->sl) {
			m->sl(hWnd, iListBoxSelectedIndex, strListBoxSelectedItem.data(), iItemHeight);
		}

		OutputDebugStream(_T("�����͕⊮�̑I�����ڂ��ύX���ꂽ���u %d, %s �v\n"), iListBoxSelectedIndex, strListBoxSelectedItem.data());
		return TRUE;
	}


	/*----------------------------------------------------------------------------------------------------------------------
	 * ���͕⊮�E�B���h�E���j������钼�O�ɌĂ΂��R�[���o�b�N�֐�
	 *----------------------------------------------------------------------------------------------------------------------
	 * hWnd                   : ���X�g�{�b�N�X�E�B���h�E
	 * (���X�g�{�b�N�X�E�B���h�E��SendMessage�Ƃ������Ȃ����ƁB�v�Z������)
	 */
	int OnDestroy(HWND hWnd) {
		OutputDebugStream(_T("�����͕⊮�E�B���h�E���j������钼�O\n"));
		if (mapBindAutoCompleteDll.size() == 0) {
			return FALSE;
		}

		BINDAUTOCOMPLETEDLL *m = GetBindingDll();
		if (!m) {
			return FALSE;
		}

		if (m->dr) {
			OutputDebugStream(_T("����dr�Ăяo��"));
			m->dr(hWnd);
		}

		return TRUE;
	}



	/*----------------------------------------------------------------------------------------------------------------------
	 * ���͕⊮���X�g�ɉ������ʂɒǉ����ׂ����ڂ�����̂��A�K�v�ƂȂ�x�ɖ₢���킹������R�[���o�b�N�֐�
	 *----------------------------------------------------------------------------------------------------------------------
	 * original_list : ���݌��ƂȂ��Ă���I���W�i���̃��X�g
	 * filename      : �t�@�C����
	 * curtext       : �ҏW�����܂߂�����̃e�L�X�g���e(���o�[�W�����ł́A�@�\���Ă��Ȃ�)
	 * cursor_param  : �J�[�\���֘A�̃p�����[�^�B���o�[�W�����ł͋@�\���Ȃ��B
	 * ���A���̏u�ԂɁu���͕⊮���ɓ��ʂɒǉ�����ׂ����X�g�v��Ԃ��B���̓��X�g�́u�Q�Ԗځv�ȍ~�ɑ}�������B
	 * �ucurtext�v�Ɓucursur_param�v�́A���݂̂Ƃ���@�\���Ȃ��B(�G�ۂŎ��Ȃ�)�B�����Ή��\��
	 */

	vector<tstring> OnQueryListBoxCustomAdded(HWND hWnd, vector<tstring>& original_list) {

		OutputDebugStream(_T("�����͕⊮�̓��ʒǉ��₢���킹\n"));
		vector<tstring> add_list;

		BINDAUTOCOMPLETEDLL *m = GetBindingDll();

		if (mapBindAutoCompleteDll.size() == 0) {
			return add_list;
		}
		if (!m) {
			return add_list;
		}
		// ����
		if (!m->ca) {
			return add_list;
		}

		// �ȉ���DLL�Ƀ��\�b�h������B
		for each(auto s in original_list) {
			OutputDebugStream(_T("�ꗗ:%s\n"), s.data());
		}

		// original_list ���̂͑S�Ă����̊֐����ł͓��Rpin_ptr�ł���̂ŁA���̂܂܃|�C���^��n����OK�ł���B
		// �u�uTCHAR *�v�̔z��v
		size_t org_size = original_list.size();
		AUTOCOMPLETELIST AutoCompleteOriginalList;

		if (org_size > AUTOCOMPLETELIST_MAX_COUNT) {
			AutoCompleteOriginalList.list_size = AUTOCOMPLETELIST_MAX_COUNT;
		}
		else {
			AutoCompleteOriginalList.list_size = org_size;
		}

		for (size_t i = 0; i < AutoCompleteOriginalList.list_size; i++) {
			AutoCompleteOriginalList.string_ptr_list[i] = LPTSTR(original_list[i].data()); // �A�h���X�̊i�[
			OutputDebugStream(_T("�����i�[ %s"), AutoCompleteOriginalList.string_ptr_list[i]);
		}

		OutputDebugStream(_T("����ca�Ăяo��"));
			
		AUTOCOMPLETELIST* AutoCompleteAdditionList = m->ca(hWnd, &AutoCompleteOriginalList);
		if (AutoCompleteAdditionList && AutoCompleteAdditionList->string_ptr_list) {
			add_list.clear();
			size_t add_size = AutoCompleteAdditionList->list_size;
				
			for (size_t i = 0; i < add_size; i++) {
				if (AutoCompleteAdditionList->string_ptr_list[i]) {
					add_list.push_back(AutoCompleteAdditionList->string_ptr_list[i]);
				}
			}
		}

		return add_list;
	}

	/*----------------------------------------------------------------------------------------------------------------------
	 * �L�[�������ꂽ��
	 * hWnd            : �Ώۂ̃E�B���h�E
	 * wParam          : ���͂��ꂽ�R�[�h�������Ă���B
	 *----------------------------------------------------------------------------------------------------------------------
	 */
	int OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam) {
		OutputDebugStream(_T("�����ꂽ�L�[%c"), wParam);
		if (mapBindAutoCompleteDll.size() == 0) {
			return FALSE;
		}

		BINDAUTOCOMPLETEDLL *m = GetBindingDll();
		if (!m) {
			return FALSE;
		}

		if (m->kd) {
			OutputDebugStream(_T("����kd�Ăяo��"));
			m->kd(hWnd, wParam);
		}

		return TRUE;
	}
}



vector<BINDAUTOCOMPLETEDLL> mapBindAutoCompleteDll; // ���K�\���ɑΉ�����dll�̓o�^


intptr_t BindAutoCompleter(TCHAR *szDllFileName, TCHAR *szRegexp) {


	if (!PathFileExists(szDllFileName)) {
		return FALSE;
	}


	// �o�^�������Ԃ��������ł�����̂ŁAmap�͎g���Ȃ��Bvector�œo�^����B
	// ���łɓo�^����Ă��邩�ǂ����́A�uregexp�v�ƁuszDllFileName�v�̑g�ݍ��킹
	bool bIsExist = false;
	for (auto m : mapBindAutoCompleteDll) {
		// ���łɓo�^����Ă���Ȃ�c
		if (m.strRegexp == szRegexp && m.strDllName == szDllFileName) {
			bIsExist = true;
		}
	}

	// ���łɓo�^����Ă����牽�����Ȃ�
	if (bIsExist) {
		return TRUE;
	}


	BINDAUTOCOMPLETEDLL dll; // �S��0�ŃN���A���Ă邱�Əd��

	dll.strRegexp = tstring(szRegexp);
	dll.strDllName = tstring(szDllFileName);

	// dll������
	HMODULE hAutoCompMod = LoadLibrary(szDllFileName);
	if (!hAutoCompMod) {
		OutputDebugStream( _T("����dll������") );
		OutputDebugStream(szDllFileName);
		return FALSE;
	}

	PFNCREATE cr = (PFNCREATE)GetProcAddress(hAutoCompMod, "OnCreate");
	if (cr) {
		dll.cr = cr;
	}

	PFNSELECT sl = (PFNSELECT)GetProcAddress(hAutoCompMod, "OnListBoxSelectedIndexChanged");
	if (sl) {
		dll.sl = sl;
	}

	PFNDESTROY dr = (PFNDESTROY)GetProcAddress(hAutoCompMod, "OnDestroy");
	if (dr) {
		dll.dr = dr;
	}

	PFNADDLIST ca = (PFNADDLIST)GetProcAddress(hAutoCompMod, "OnQueryListBoxCustomAdded");
	if (ca) {
		dll.ca = ca;
	}

	PFNKEYDOWN	kd = (PFNKEYDOWN)GetProcAddress(hAutoCompMod, "OnKeyDown");
	if (kd) {
		dll.kd = kd;
	}

	mapBindAutoCompleteDll.push_back(dll);
	return TRUE;
}
