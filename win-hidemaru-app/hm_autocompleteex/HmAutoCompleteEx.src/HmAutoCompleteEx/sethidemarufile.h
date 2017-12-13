/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#pragma once

#include <windows.h>
#include "tstring.h"

using namespace std;

extern TCHAR szCurrentFileFullPath[MAX_PATH * 2];

extern TCHAR *GetCurrentFileName();
