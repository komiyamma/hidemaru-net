#include <shlwapi.h>

#include "perlez_engine.h"
#include "self_dll_info.h"
#include "convert_string.h"
#include "output_debugstream.h"

#include <vector>

using namespace std;

#pragma comment(lib, "shlwapi.lib")

// static�̏�����
HMODULE CPerlEzEngine::hPerlEzDll = NULL;
PERLEZHANDLE CPerlEzEngine::engine = NULL;

CPerlEzEngine::CPerlEzEngine() {

	//-------------------------------------------------------------------------
	// ActivePerl�Ȃ񂾂���Path���ʂ��Ă����ˁH �Ƃ����O��̎d�g�݂ł� >> PerlEz
	//-------------------------------------------------------------------------
	hPerlEzDll = LoadLibrary(L"PERLEZ.DLL");
	if (hPerlEzDll) {

		//-------------------------------------------------------------------------
		// �ȉ���10�N���炢�ύX�������̂ŁA�`�F�b�N���邾�����ʂȂ̂ŁA�֐��͑��݂��Ă�ƐM����B
		//-------------------------------------------------------------------------
		this->PerlEzCreate = (TPerlEzCreate)GetProcAddress(hPerlEzDll, "PerlEzCreate");
		this->PerlEzDelete = (TPerlEzDelete)GetProcAddress(hPerlEzDll, "PerlEzDelete");

		this->PerlEzCall = (TPerlEzCall)GetProcAddress(hPerlEzDll, "PerlEzCall");
		this->PerlEzEvalString = (TPerlEzEvalString)GetProcAddress(hPerlEzDll, "PerlEzEvalString");

		this->PerlEzSetMagicScalarName = (TPerlEzSetMagicScalarName)GetProcAddress(hPerlEzDll, "PerlEzSetMagicScalarName");
		this->PerlEzSetMagicScalarFunctions = (TPerlEzSetMagicScalarFunctions)GetProcAddress(hPerlEzDll, "PerlEzSetMagicScalarFunctions");


		//-------------------------------------------------------------------------
		// dll�̖��O�ɑΉ�����`�ŁA.pm�̖��O�����߂�B
		// �f�t�H���g���ƁAhmPerl.dll��hmPerl.pm�ƂȂ�

		//-------------------------------------------------------------------------
		// �������g��dll�̖��O���܂��̓R�s�[���āc
		_tcscpy_s(szPerlCreateFileFullPath, CSelfDllInfo::szSelfModuleFullPath);
		//-------------------------------------------------------------------------
		// �g���q��.pm�ւƕύX�������̂��A�ǂ݂��ށB
		PathRenameExtension(szPerlCreateFileFullPath, L".pm");

		//-------------------------------------------------------------------------
		// PerlEz���Â����炠��̂ŁAascii�Ƃ������}���`�o�C�g�̃t�@�C�������ǂݍ��߂Ȃ�
		if (PathFileExists(szPerlCreateFileFullPath)) {
			string cp932_full_path = utf16_to_cp932(szPerlCreateFileFullPath);
			this->engine = this->PerlEzCreate(cp932_full_path.data(), NULL);
			if (!this->engine) {
				wstring errmsg = wstring(szPerlCreateFileFullPath) + L"\n�֘A�Ńg���u�����������܂����B";
				MessageBox(NULL, errmsg.data(), errmsg.data(), MB_ICONERROR);
				OutputDebugStream(errmsg);
			}
		}
	}

	else {
		MessageBox(NULL, L"ActivePerl x86�ł�F���ł��܂���", L"ActivePerl x86�ł�F���ł��܂���", MB_ICONERROR);
	}
}


CPerlEzEngine::~CPerlEzEngine() {

	//-------------------------------------------------------------------------
	// �G���W����j�����Ă���c
	//-------------------------------------------------------------------------
	if (engine) {
		PerlEzDelete(engine);
		engine = NULL;
	}

	//-------------------------------------------------------------------------
	// dll��j���B
	//-------------------------------------------------------------------------
	if (hPerlEzDll) {
		FreeLibrary(hPerlEzDll);
		hPerlEzDll = NULL;
	}
}


BOOL CPerlEzEngine::IsValid() {
	return (hPerlEzDll != NULL);
}


BOOL CPerlEzEngine::IsReturnBufferSeemsEvalError(char *utf8_buffer) {
	// �����G���[���ۂ������񂪓����Ă��邼�c
	if (strstr(utf8_buffer, "(eval") && strstr(utf8_buffer, "at (") && strstr(utf8_buffer, ") line")) {
		return true;
	}
	return false;
}


// ��������w�蕶���ŕ���
static vector<wstring> wstring_split(const std::wstring& s, const std::wstring& delim)
{
	vector<wstring> result;
	result.clear();

	using wstring = std::wstring;
	wstring::size_type pos = 0;

	while (pos != wstring::npos)
	{
		wstring::size_type p = s.find(delim, pos);

		if (p == wstring::npos)
		{
			result.push_back(s.substr(pos));
			break;
		}
		else {
			result.push_back(s.substr(pos, p - pos));
		}

		pos = p + delim.size();
	}

	return result;
}


// �G���[�̈ʒu�̏����o��
CPerlEzEngine::TEvalErrorInfo CPerlEzEngine::GetEvalErrorInfo(wstring strAllSrcCode, char *utf8_buffer) {
	TEvalErrorInfo errInfo;

	// �����G���[���ۂ������񂪓����Ă��邼�c
	if (this->IsReturnBufferSeemsEvalError(utf8_buffer)) {

		//-------------------------------------------------------------------------
		// ���K�\���ŁA���m�ɍs�ƈʒu�𒊏o����
		//-------------------------------------------------------------------------
		//  at (eval 9) line 2
		std::wregex wre( L"at \\(eval (\\d+)\\) line (\\d+)" );

		std::wsmatch wmt; // match_results

		auto result = std::regex_search(utf8_to_utf16(utf8_buffer), wmt, wre);
		wstring w1 = wmt[1];
		wstring w2 = wmt[2];

		//-------------------------------------------------------------------------
		// ���l�ɕϊ��ł��邩�ǂ����g���C
		//-------------------------------------------------------------------------
		int column = -1;
		int lineno = -1;
		try {
			column = std::stoi(w1);
			lineno = std::stoi(w2);
		}
		catch (...) {
			column = -1;
			lineno = -1;
		}

		//-------------------------------------------------------------------------
		// Perl�R�[�h�S�̂����s�ŕ������āc
		//-------------------------------------------------------------------------
		auto result_list = wstring_split(strAllSrcCode, L"\n");

		//-------------------------------------------------------------------------
		// �G���[���������Ƃ����s�̕���������Ă���
		//-------------------------------------------------------------------------
		int n = lineno - 1;
		if (0 <= n && n < (int)result_list.size()) {
			errInfo.errorlinecode = result_list[n];
		}

		errInfo.column = column;
		errInfo.lineno = lineno;
		errInfo.HasError = true;
		
	}
	else {
		errInfo.HasError = false;
	}
	return errInfo;
}