#include <windows.h>

#include "hidemaruexe_export.h"

#pragma comment(lib, "version.lib")

HMODULE CHidemaruExeExport::hHideExeHandle = NULL;
wchar_t CHidemaruExeExport::szHidemaruFullPath[MAX_PATH] = L"";

CHidemaruExeExport::PFNGetDllFuncCalledType CHidemaruExeExport::Hidemaru_GetDllFuncCalledType = NULL;
CHidemaruExeExport::PFNGetTotalTextUnicode CHidemaruExeExport::Hidemaru_GetTotalTextUnicode = NULL;
CHidemaruExeExport::PFNGetSelectedTextUnicode CHidemaruExeExport::Hidemaru_GetSelectedTextUnicode = NULL;
CHidemaruExeExport::PFNGetLineTextUnicode CHidemaruExeExport::Hidemaru_GetLineTextUnicode = NULL;
CHidemaruExeExport::PFNGetCursorPosUnicode CHidemaruExeExport::Hidemaru_GetCursorPosUnicode = NULL;
CHidemaruExeExport::PFNGetCursorPosUnicodeFromMousePos CHidemaruExeExport::Hidemaru_GetCursorPosUnicodeFromMousePos = NULL;
CHidemaruExeExport::PFNEvalMacro CHidemaruExeExport::Hidemaru_EvalMacro = NULL;

double CHidemaruExeExport::hm_version = 0;
double CHidemaruExeExport::QueryFileVersion(wchar_t* path){
	VS_FIXEDFILEINFO* v;
	DWORD dwZero = 0;
	UINT len;
	DWORD sz = GetFileVersionInfoSize(path, &dwZero);
	if (sz){
		void* buf = new char[sz];
		GetFileVersionInfo(path, dwZero, sz, buf);

		if (VerQueryValue(buf, L"\\", (LPVOID*)&v, &len)){
			double ret = 0;
			ret = double(HIWORD(v->dwFileVersionMS)) * 100 +
				double(LOWORD(v->dwFileVersionMS)) * 10 +
				double(HIWORD(v->dwFileVersionLS)) +
				double(LOWORD(v->dwFileVersionLS)) * 0.01;
			delete buf;
			return ret;
		}
		else{
			delete buf;
		}
	}

	return 0;
}


BOOL CHidemaruExeExport::init() {

	if (hHideExeHandle) {
		return TRUE;
	}

	GetModuleFileName(NULL, szHidemaruFullPath, _countof(szHidemaruFullPath));
	hm_version = QueryFileVersion(szHidemaruFullPath);

	if (hm_version < 866) {
		MessageBox(NULL, L"�G�ۂ̃o�[�W�������s�����Ă��܂��B\n�G�ۃG�f�B�^ v8.66�ȏオ�K�v�ł��B", L"���", MB_ICONERROR);
		return FALSE;
	}

	// �G�ۖ{�̂Ɋ֐�������̂� 8.66�ȏ�
	hHideExeHandle = LoadLibrary(szHidemaruFullPath);

	if (hHideExeHandle) {
		Hidemaru_GetDllFuncCalledType = (PFNGetDllFuncCalledType)GetProcAddress(hHideExeHandle, "Hidemaru_GetDllFuncCalledType");
		Hidemaru_GetTotalTextUnicode = (PFNGetTotalTextUnicode)GetProcAddress(hHideExeHandle, "Hidemaru_GetTotalTextUnicode");
		Hidemaru_GetSelectedTextUnicode = (PFNGetSelectedTextUnicode)GetProcAddress(hHideExeHandle, "Hidemaru_GetSelectedTextUnicode");
		Hidemaru_GetLineTextUnicode = (PFNGetLineTextUnicode)GetProcAddress(hHideExeHandle, "Hidemaru_GetLineTextUnicode");
		Hidemaru_GetCursorPosUnicode = (PFNGetCursorPosUnicode)GetProcAddress(hHideExeHandle, "Hidemaru_GetCursorPosUnicode");
		Hidemaru_GetCursorPosUnicodeFromMousePos = (PFNGetCursorPosUnicodeFromMousePos)GetProcAddress(hHideExeHandle, "Hidemaru_GetCursorPosUnicodeFromMousePos");
		Hidemaru_EvalMacro = (PFNEvalMacro)GetProcAddress(hHideExeHandle, "Hidemaru_EvalMacro");
		return TRUE;
	}

	return FALSE;
}


wstring CHidemaruExeExport::GetTotalText() {
	HGLOBAL hGlobal = CHidemaruExeExport::Hidemaru_GetTotalTextUnicode();
	if (hGlobal) {
		wchar_t* pwsz = (wchar_t*)GlobalLock(hGlobal);
		wstring text(pwsz); // �R�s�[
		GlobalUnlock(hGlobal);
		GlobalFree(hGlobal); // ���͉̂��
		return text;
	}
	return L"";
}


wstring CHidemaruExeExport::GetSelectedText() {
	HGLOBAL hGlobal = CHidemaruExeExport::Hidemaru_GetSelectedTextUnicode();
	if (hGlobal) {
		wchar_t* pwsz = (wchar_t*)GlobalLock(hGlobal);
		wstring text(pwsz); // �R�s�[
		GlobalUnlock(hGlobal);
		GlobalFree(hGlobal); // ���͉̂��
		return text;
	}
	return L"";
}

wstring CHidemaruExeExport::GetLineText(int lineno) {
	// ���C���i���o�[�̎w�肪�Ȃ���΁A���݂̃J�[�\����lineno
	if (lineno == 0) {
		auto pos = CHidemaruExeExport::GetCursorPos();
		lineno = pos.lineno;
	}
	HGLOBAL hGlobal = CHidemaruExeExport::Hidemaru_GetLineTextUnicode(lineno);
	if (hGlobal) {
		wchar_t* pwsz = (wchar_t*)GlobalLock(hGlobal);
		wstring text(pwsz); // �R�s�[
		GlobalUnlock(hGlobal);
		GlobalFree(hGlobal); // ���͉̂��
		return text;
	}
	return L"";
}


CHidemaruExeExport::HmCursurPos CHidemaruExeExport::GetCursorPos() {
	int nLineNo = -1;
	int nColumn = -1;
	BOOL success = Hidemaru_GetCursorPosUnicode(&nLineNo, &nColumn);
	if (!success) {
		nLineNo = -1;
		nColumn = -1;
	}
	HmCursurPos pos(nLineNo, nColumn);
	return pos;
}

CHidemaruExeExport::HmMousePos CHidemaruExeExport::GetCursorPosFromMousePos() {
	POINT point;
	int s = ::GetCursorPos(&point);
	if (!s) {
		point.x = -1;
		point.y = -1;
	}
	int nLineNo = -1;
	int nColumn = -1;

	// �Y���̊֐������݂��Ă��鎞�����l���X�V(�G�� 8.73�ȏ�)
	if (Hidemaru_GetCursorPosUnicodeFromMousePos) {
		// ����success��nLineNo�������́AnColumn�̂ǂ��炩�P�����s�����False��Ԃ��Ă��܂��̂ŁA�Ԃ�l�͎g��Ȃ�
		BOOL _ = Hidemaru_GetCursorPosUnicodeFromMousePos(NULL, &nLineNo, &nColumn);
	}
	HmMousePos pos(point.x, point.y, nLineNo, nColumn);
	return pos;
}


BOOL CHidemaruExeExport::EvalMacro(wstring cmd) {
	return Hidemaru_EvalMacro(cmd.data());
}
