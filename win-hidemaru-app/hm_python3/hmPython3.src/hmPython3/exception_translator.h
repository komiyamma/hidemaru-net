#pragma once

#include <windows.h>
#include <string>


class CSEHException {
public:
	CSEHException(
		unsigned int ExceptionCode,
		EXCEPTION_POINTERS* ExceptionPointers)
		: m_ExceptionCode(ExceptionCode),
		m_ExceptionPointers(ExceptionPointers) {}

	std::wstring what();

private:
	unsigned int m_ExceptionCode;
	EXCEPTION_POINTERS* m_ExceptionPointers;

};



void PythonTransSEHtoCEH(unsigned int ExceptionCode, PEXCEPTION_POINTERS ExceptionPointers);
