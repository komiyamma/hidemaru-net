#pragma once

#include <windows.h>

#define RECIEVEBUTTON_ID 50001

extern HWND hRecieveButton;


HWND CreateRecieveButton(HWND hWnd, HINSTANCE hInst);
void DeleteRecieveButton();
HFONT SetButtonFont(LPCTSTR face, int h, int angle);
void SwitchRecieveStatus();