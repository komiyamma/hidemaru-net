#pragma once

#include "python_hidemaru_lib.h"




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


extern void PythonAlreadySetException(py::error_already_set &e);
extern void PythonKnknownException(exception &e);
extern void SystemAlreadySetException(CSEHException &e);
extern void SystemUnknownException();
