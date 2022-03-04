// dllmain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
#include <windows.h>
#include <vector>
#include <iostream>
#include <string>
#pragma warning(suppress : 4996)

using namespace std;

int g_WcharCode = -1;
wchar_t wstrWcharString[2] = L"";
extern "C" __declspec(dllexport) wchar_t* GetUnincodeNext(int) {
    g_WcharCode++;
    if (g_WcharCode > 0xFFFF) {
        return (wchar_t*)L"END";
    }
    else {
        wchar_t tmp = (wchar_t)g_WcharCode;
        wstrWcharString[0] = tmp;
        wstrWcharString[1] = NULL;
        return wstrWcharString;
    }
}
FILE* outputfile = NULL;         // 出力ストリーム
void write_file(int i, unsigned int b) {
    if (outputfile == NULL) {
#pragma warning(suppress : 4996)
        outputfile = fopen("HmMakeUniqueUnicodeMapFile.txt", "w");  // ファイルを書き込み用にオープン(開
    }

    wstring s =/* std::to_wstring(i) + L"," + */std::to_wstring(b) + L"\n";
    fwprintf(outputfile, s.data()); // ファイルに書く


}
int ai = 0;
extern "C" __declspec(dllexport) intptr_t ConvertWStrToHmUniqueUnicodeCode(const unsigned char* s)
{
    vector<int> v;

    union A {
        unsigned int i;
        char ch[4];
    };

    A a;
    a.i = 0;
    a.ch[0] = 0;
    a.ch[1] = 0;
    a.ch[2] = 0;
    a.ch[3] = 0;


    for (int i = 0; true; i++) {
        if (s[i] == 0) {
            break;
        }

        v.push_back((unsigned char)s[i]);
        a.ch[i] = s[i];
    }
    ai = a.i;
    write_file(g_WcharCode, a.i);
    return 0;
}



extern "C" __declspec(dllexport) intptr_t STARTUNIMACRO() {
    return (intptr_t)1;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

