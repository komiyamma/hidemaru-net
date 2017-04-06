#pragma once

#include <windows.h>
#include "tstring.h"

using namespace std;

extern TCHAR szCurrentFileFullPath[MAX_PATH * 2];

extern TCHAR *GetCurrentFileName();
