#include "SharedExport.h"

HMODULE hProcess = NULL;
PFNOUTPUT pOutputFunc = NULL;
PFNGETWINDOWHANDLE pOutputGetWindowFunc = NULL;
HMODULE hHmOutputPaneDLL = NULL;

