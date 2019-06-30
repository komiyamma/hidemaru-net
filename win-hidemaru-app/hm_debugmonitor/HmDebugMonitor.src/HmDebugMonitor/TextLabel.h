/*
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the MIT License
 */

#include <windows.h>

// 正規表現受信フィルター
void CreateRegexBoxTextLabel(HWND hWnd);
void DeleteRegexBoxTextLabel();
HFONT SetLabelFont(LPCTSTR face, int h, int angle);