/*
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the MIT License
 */

#pragma once

#include <windows.h>
#include <richedit.h>

const int RegexBoxWidth = 150;

extern HWND hRgxEdit;

extern char hRgxEditContent[4096]; // ���K�\���̈悩��e�L�X�g�𒊏o

HWND CreateRegexBox(HWND hWnd, HINSTANCE hInst);
void DeleteRegexBox();
BOOL SetInitialFont(HWND hWnd);
void MoveRegexBox(HWND hWnd);