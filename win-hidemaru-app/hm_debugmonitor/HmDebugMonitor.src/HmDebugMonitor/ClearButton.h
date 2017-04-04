#pragma once

#include <windows.h>

#define CLEARBUTTON_ID 50005

extern HWND hClearButton;

const int ClearButtonPosX = 95;

HWND CreateClearButton(HWND hWnd, HINSTANCE hInst);
void DeleteClearButton();
HFONT SetButtonFont(LPCTSTR face, int h, int angle);
