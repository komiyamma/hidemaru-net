#pragma unmanaged

#include <windows.h>

#if defined(_WIN64)

#pragma comment(lib, "EverythingStaticLib_x64.lib")

#else 

#pragma comment(lib, "EverythingStaticLib_x86.lib")

#endif



