/*
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the MIT License
 */

#pragma once

#include <windows.h>

#define CLEARBUTTON_ID 50005

extern HWND hClearButton;

const int ClearButtonPosX = 95;

HWND CreateClearButton(HWND hWnd, HINSTANCE hInst);
void DeleteClearButton();
HFONT SetButtonFont(LPCTSTR face, int h, int angle);
