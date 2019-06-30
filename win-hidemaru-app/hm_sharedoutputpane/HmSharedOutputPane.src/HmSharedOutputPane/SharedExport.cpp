/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#include "SharedExport.h"

HMODULE hProcess = NULL;
PFNOUTPUT pOutputFunc = NULL;
PFNGETWINDOWHANDLE pOutputGetWindowFunc = NULL;
HMODULE hHmOutputPaneDLL = NULL;

