/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#pragma once

#include <windows.h>

extern HWND hWndAutoCompleteListBox;
extern WPARAM wLastKeyDown;
extern int iPrevAutoCompleteListBoxIndex;

BOOL OnHidemaruListBoxKeyDown(HWND hWnd, WPARAM wparam);
BOOL OnHidemaruListBoxAppear(HWND hWnd, WPARAM wparam);

BOOL OnHidemaruApplicationKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
BOOL ResetAutoCompleteListBoxIndex();

using PFNPROC = LRESULT(CALLBACK *)(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern PFNPROC pOriginalWIndowProcAutoCompleteListBox;

extern BOOL AddCompleteString(HWND, WPARAM);


extern LRESULT CALLBACK WindowProcAutoCompleteListBox(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

extern BOOL bAutoCompleteListBoxAddMode;

extern BOOL OnHidemaruListBoxAddCompletePrev(HWND hListBox, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern BOOL OnHidemaruListBoxAddCompletePost(HWND hListBox, UINT uMsg, WPARAM wParam, LPARAM lParam);

// èGä€Ç™ç≈èâÇÃÉèÅ[ÉhÇ…ëŒÇµÇƒì¡ï Ç…ë´ÇµçûÇﬁÇ‚Ç¬
#define AUTOCOMPLETE_HIDEMARU_ESPECIAL_ADD_WORD_REGEX _T("\\s+\\(.+?=.+?\\)")