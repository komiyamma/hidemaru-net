#pragma once


#define MACRO_DLL extern "C" __declspec(dllexport)


#define intHM_t intptr_t



//========================================================================
/// ���̃t�@�C�����猩�Ă��邽�߂�extern�B���ۂ�.cpp�̕����݂Ă�
//========================================================================
MACRO_DLL intHM_t GetNumVar(const wchar_t *utf16_simbol);
MACRO_DLL intHM_t SetNumVar(const wchar_t *utf16_simbol, intHM_t value);
MACRO_DLL const wchar_t * GetStrVar(const wchar_t *utf16_simbol);
MACRO_DLL intHM_t SetStrVar(const wchar_t *utf16_simbol, const wchar_t *utf16_value);
